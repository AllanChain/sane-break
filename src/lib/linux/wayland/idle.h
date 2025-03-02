// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#ifndef SANE_IDLE_WAYLAND_H
#define SANE_IDLE_WAYLAND_H

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QElapsedTimer>
#include <QTimer>
#include <cstdint>

#include "idle-time.h"  // IWYU pragma: export
#include "wayland-ext-idle-notify-v1-client-protocol.h"

class IdleTimeWayland : public SystemIdleTime {
  Q_OBJECT
 public:
  IdleTimeWayland();
  ~IdleTimeWayland();
  bool isSupported() { return idleNotifier != nullptr; };
  void startWatching();
  void stopWatching();
  void setWatchAccuracy(int accuracy) {};
  void setMinIdleTime(int idleTime);

 private:
  static void globalAdded(void *data, wl_registry *registry, uint32_t name,
                          const char *interface, uint32_t version);
  static void globalRemoved(void *data, wl_registry *registry, uint32_t name);
  const struct wl_registry_listener globalListener {
    globalAdded, globalRemoved
  };
  static void idled(void *data, ext_idle_notification_v1 *object);
  static void resumed(void *data, ext_idle_notification_v1 *object);
  const struct ext_idle_notification_v1_listener idleListener {
    idled, resumed
  };
  wl_seat *seat;
  ext_idle_notifier_v1 *idleNotifier = nullptr;
  ext_idle_notification_v1 *idleNotification = nullptr;

  struct ext_idle_notification_v1 *(*get_idle_notification)(
      struct ext_idle_notifier_v1 *ext_idle_notifier_v1, uint32_t timeout,
      struct wl_seat *seat);

  bool isWatching = false;
};

class IdleTimeGNOME : public ReadBasedIdleTime {
  Q_OBJECT
 public:
  IdleTimeGNOME();
  int systemIdleTime();

 private:
  QDBusInterface *iface;
};

#endif  // SANE_IDLE_WAYLAND_H
#endif  // Q_OS_LINUX
