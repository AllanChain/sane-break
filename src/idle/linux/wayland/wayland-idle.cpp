// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wayland-idle.h"

#include <qglobal.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <QDebug>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QObject>
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
#include <QtGui/qpa/qplatformnativeinterface.h>
#endif
#include <cstdint>
#include <cstring>

#include "core/idle-time.h"
#include "wayland-ext-idle-notify-v1-client-protocol.h"

IdleTimeWayland::IdleTimeWayland(QObject* parent) {
  setParent(parent);
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  QNativeInterface::QWaylandApplication* waylandApp =
      qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  if (!waylandApp) return;
  wl_display* display = waylandApp->display();
  seat = waylandApp->seat();
#else
  QPlatformNativeInterface* nativeInterface = qGuiApp->platformNativeInterface();
  if (!nativeInterface) return;
  wl_display* display = static_cast<wl_display*>(
      nativeInterface->nativeResourceForIntegration("display"));
  seat =
      static_cast<wl_seat*>(nativeInterface->nativeResourceForIntegration("wl_seat"));
#endif
  wl_registry* registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &globalListener, this);
  wl_display_roundtrip(display);

  chooseGetIdleNotification();
}

void IdleTimeWayland::chooseGetIdleNotification() {
  if (idleNotifier == nullptr) return;
  const auto wl_ntfr_ver = ext_idle_notifier_v1_get_version(idleNotifier);
  // InhibitorAware (default): use v1 get_idle_notification, which respects
  // zwp_idle_inhibitor_v1. InputOnly: use v2 get_input_idle_notification, which
  // ignores inhibitors; fall back to v1 on v1-only compositors.
  if (m_idleMode == IdleMode::InhibitorAware || wl_ntfr_ver < 2) {
    get_idle_notification = ext_idle_notifier_v1_get_idle_notification;
    qDebug() << "idle: wayland using v1 get_idle_notification (inhibitor-aware)";
  } else {
    get_idle_notification = ext_idle_notifier_v1_get_input_idle_notification;
    qDebug() << "idle: wayland using v2 get_input_idle_notification (input-only)";
  }
}

void IdleTimeWayland::globalAdded(void* data, wl_registry* registry, uint32_t name,
                                  const char* interface, uint32_t version) {
  IdleTimeWayland* self = static_cast<IdleTimeWayland*>(data);
  if (strcmp(interface, ext_idle_notifier_v1_interface.name) == 0) {
    if (self->idleNotifier != nullptr)  // Clear old notifier
      ext_idle_notifier_v1_destroy(self->idleNotifier);
    self->idleNotifier = static_cast<struct ext_idle_notifier_v1*>(
        wl_registry_bind(registry, name, &ext_idle_notifier_v1_interface, version));
  }
}

void IdleTimeWayland::globalRemoved(void*, wl_registry*, uint32_t) {};

void IdleTimeWayland::idled(void* data, ext_idle_notification_v1* object) {
  IdleTimeWayland* self = static_cast<IdleTimeWayland*>(data);
  if (!self->isWatching) return;
  // The pending-resume request already fired idled; it only fires resumed from here.
  if (object == self->pendingResumeNotification) return;
  if (object != self->idleNotification) return;  // stale object
  if (self->pendingResumeNotification != nullptr) {
    // The new request re-confirmed idle; the old request's resume is redundant now.
    ext_idle_notification_v1_destroy(self->pendingResumeNotification);
    self->pendingResumeNotification = nullptr;
  }
  if (!self->m_isIdle) {
    self->m_isIdle = true;
    emit self->idleStart();
  }
};

void IdleTimeWayland::resumed(void* data, ext_idle_notification_v1* object) {
  IdleTimeWayland* self = static_cast<IdleTimeWayland*>(data);
  if (!self->isWatching) return;
  if (object == self->pendingResumeNotification) {
    // User resumed before the new request re-confirmed idle: report active now.
    ext_idle_notification_v1_destroy(self->pendingResumeNotification);
    self->pendingResumeNotification = nullptr;
    self->m_isIdle = false;
    emit self->idleEnd();
    return;
  }
  if (object != self->idleNotification) return;  // stale object
  if (self->pendingResumeNotification != nullptr) {
    // Defensive: never keep two requests that can report resume.
    ext_idle_notification_v1_destroy(self->pendingResumeNotification);
    self->pendingResumeNotification = nullptr;
  }
  self->m_isIdle = false;
  emit self->idleEnd();
};

void IdleTimeWayland::startWatching() {
  if (idleNotifier == nullptr) return;
  isWatching = true;
  m_isIdle = false;
  if (pendingResumeNotification != nullptr) {
    ext_idle_notification_v1_destroy(pendingResumeNotification);
    pendingResumeNotification = nullptr;
  }
  idleNotification = get_idle_notification(idleNotifier, m_minIdleTime, seat);
  ext_idle_notification_v1_add_listener(idleNotification, &idleListener, this);
}

void IdleTimeWayland::stopWatching() {
  isWatching = false;
  if (pendingResumeNotification != nullptr) {
    ext_idle_notification_v1_destroy(pendingResumeNotification);
    pendingResumeNotification = nullptr;
  }
  if (idleNotification != nullptr) {
    ext_idle_notification_v1_destroy(idleNotification);
    idleNotification = nullptr;
  }
}

void IdleTimeWayland::setIdleDetection(int idleTime, IdleMode mode) {
  const bool idleTimeChanged = idleTime != m_minIdleTime;
  const bool modeChanged = mode != m_idleMode;
  if (!idleTimeChanged && !modeChanged) return;
  m_minIdleTime = idleTime;
  m_idleMode = mode;
  // Re-evaluate which request to use on a mode change, then re-create the
  // notification object so the new settings (v1 vs v2 request, timeout) take
  // effect immediately. Doing both changes in one re-creation avoids arming an
  // intermediate request with a stale combination of mode and timeout.
  if (modeChanged) chooseGetIdleNotification();
  if (!isWatching) return;
  recreateNotification();
}

void IdleTimeWayland::recreateNotification() {
  // Keep the old request alive while idle so a pending resume isn't dropped (see
  // the pattern comment on the members in the header).
  if (m_isIdle) {
    if (pendingResumeNotification == nullptr) {
      // The current request already fired idled; keep it for the pending resume.
      pendingResumeNotification = idleNotification;
      idleNotification = nullptr;
    } else if (idleNotification != nullptr) {
      // Another switch before confirmation: discard the intermediate request.
      ext_idle_notification_v1_destroy(idleNotification);
      idleNotification = nullptr;
    }
  } else {
    if (pendingResumeNotification != nullptr) {
      // Defensive: no pending resume when the user is not idle.
      ext_idle_notification_v1_destroy(pendingResumeNotification);
      pendingResumeNotification = nullptr;
    }
    if (idleNotification != nullptr) {
      ext_idle_notification_v1_destroy(idleNotification);
      idleNotification = nullptr;
    }
  }
  idleNotification = get_idle_notification(idleNotifier, m_minIdleTime, seat);
  ext_idle_notification_v1_add_listener(idleNotification, &idleListener, this);
}

IdleTimeWayland::~IdleTimeWayland() {
  if (idleNotifier != nullptr) ext_idle_notifier_v1_destroy(idleNotifier);
  if (pendingResumeNotification != nullptr)
    ext_idle_notification_v1_destroy(pendingResumeNotification);
  if (idleNotification != nullptr) ext_idle_notification_v1_destroy(idleNotification);
}
