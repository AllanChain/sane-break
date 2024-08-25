// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <wayland-client.h>

#include <QElapsedTimer>
#include <QGuiApplication>

#include "idle.h"
#include "wayland-ext-idle-notify-v1-client-protocol.h"

IdleTimeWayland::IdleTimeWayland() : SystemIdleTime() {
  QNativeInterface::QWaylandApplication *waylandApp =
      qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  wl_display *display = waylandApp->display();
  wl_registry *registry = wl_display_get_registry(display);
  elapseTimer = new QElapsedTimer();
  seat = waylandApp->seat();
  wl_registry_add_listener(registry, &globalListener, this);
  wl_display_roundtrip(display);
}

void IdleTimeWayland::globalAdded(void *data, wl_registry *registry,
                                  uint32_t name, const char *interface,
                                  uint32_t version) {
  IdleTimeWayland *self = static_cast<IdleTimeWayland *>(data);
  if (strcmp(interface, ext_idle_notifier_v1_interface.name) == 0) {
    self->idleNotifier =
        static_cast<struct ext_idle_notifier_v1 *>(wl_registry_bind(
            registry, name, &ext_idle_notifier_v1_interface, version));
  }
}

void IdleTimeWayland::globalRemoved(void *data, wl_registry *registry,
                                    uint32_t name) {
  IdleTimeWayland *self = static_cast<IdleTimeWayland *>(data);
  if (self->idleNotifier) ext_idle_notifier_v1_destroy(self->idleNotifier);
};

void IdleTimeWayland::idled(void *data, ext_idle_notification_v1 *object) {
  IdleTimeWayland *self = static_cast<IdleTimeWayland *>(data);
  self->elapseTimer->start();
  emit self->idleStart();
};

void IdleTimeWayland::resumed(void *data, ext_idle_notification_v1 *object) {
  IdleTimeWayland *self = static_cast<IdleTimeWayland *>(data);
  emit self->idleEnd(self->elapseTimer->restart());
};

void IdleTimeWayland::startWatching(WatchOption option) {
  isWatching = true;
  idleNotification = ext_idle_notifier_v1_get_idle_notification(
      idleNotifier, minIdleTime, seat);
  ext_idle_notification_v1_add_listener(idleNotification, &idleListener, this);
}

void IdleTimeWayland::stopWatching() {
  isWatching = false;
  ext_idle_notification_v1_destroy(idleNotification);
}

#endif  // Q_OS_LINUX
