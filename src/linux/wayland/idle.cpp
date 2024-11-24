// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <wayland-client.h>

#include <QGuiApplication>

#include "idle.h"
#include "wayland-ext-idle-notify-v1-client-protocol.h"

IdleTimeWayland::IdleTimeWayland() : SystemIdleTime() {
  QNativeInterface::QWaylandApplication *waylandApp =
      qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  wl_display *display = waylandApp->display();
  wl_registry *registry = wl_display_get_registry(display);
  seat = waylandApp->seat();
  wl_registry_add_listener(registry, &globalListener, this);
  wl_display_roundtrip(display);
}

void IdleTimeWayland::globalAdded(void *data, wl_registry *registry, uint32_t name,
                                  const char *interface, uint32_t version) {
  IdleTimeWayland *self = static_cast<IdleTimeWayland *>(data);
  if (strcmp(interface, ext_idle_notifier_v1_interface.name) == 0) {
    if (self->idleNotifier != nullptr)  // Clear old notifier
      ext_idle_notifier_v1_destroy(self->idleNotifier);
    self->idleNotifier = static_cast<struct ext_idle_notifier_v1 *>(
        wl_registry_bind(registry, name, &ext_idle_notifier_v1_interface, version));
  }
}

void IdleTimeWayland::globalRemoved(void *data, wl_registry *registry, uint32_t name) {
};

void IdleTimeWayland::idled(void *data, ext_idle_notification_v1 *object) {
  IdleTimeWayland *self = static_cast<IdleTimeWayland *>(data);
  if (!self->isWatching) return;
  self->isIdle = true;
  emit self->idleStart();
};

void IdleTimeWayland::resumed(void *data, ext_idle_notification_v1 *object) {
  IdleTimeWayland *self = static_cast<IdleTimeWayland *>(data);
  if (!self->isWatching) return;
  self->isIdle = false;
  emit self->idleEnd();
};

void IdleTimeWayland::startWatching() {
  isWatching = true;
  idleNotification =
      ext_idle_notifier_v1_get_idle_notification(idleNotifier, m_minIdleTime, seat);
  ext_idle_notification_v1_add_listener(idleNotification, &idleListener, this);
}

void IdleTimeWayland::stopWatching() {
  isWatching = false;
  if (idleNotification != nullptr) ext_idle_notification_v1_destroy(idleNotification);
}

void IdleTimeWayland::setMinIdleTime(int idleTime) {
  if (idleTime == m_minIdleTime) return;
  m_minIdleTime = idleTime;
  if (!isWatching) return;
  if (idleNotification != nullptr) ext_idle_notification_v1_destroy(idleNotification);
  idleNotification =
      ext_idle_notifier_v1_get_idle_notification(idleNotifier, m_minIdleTime, seat);
  ext_idle_notification_v1_add_listener(idleNotification, &idleListener, this);
}

IdleTimeWayland::~IdleTimeWayland() {
  if (idleNotifier != nullptr) ext_idle_notifier_v1_destroy(idleNotifier);
  if (idleNotification != nullptr) ext_idle_notification_v1_destroy(idleNotification);
}

#endif  // Q_OS_LINUX
