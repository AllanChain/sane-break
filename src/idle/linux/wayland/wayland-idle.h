// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <QElapsedTimer>
#include <QObject>
#include <QTimer>
#include <cstdint>

#include "core/idle-time.h"
#include "idle/idle-interface.h"
#include "wayland-ext-idle-notify-v1-client-protocol.h"

class IdleTimeWayland : public IdleTimeInterface {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID IdleTimeInterface_iid)
  Q_INTERFACES(IdleTimeInterface)
 public:
  IdleTimeWayland(QObject* parent = nullptr);
  ~IdleTimeWayland();
  bool isSupported() { return idleNotifier != nullptr; };
  void startWatching() override;
  void stopWatching() override;
  void setWatchAccuracy(int) override {};
  void setIdleDetection(int idleTime, IdleMode mode) override;

 private:
  static void globalAdded(void* data, wl_registry* registry, uint32_t name,
                          const char* interface, uint32_t version);
  static void globalRemoved(void* data, wl_registry* registry, uint32_t name);
  const struct wl_registry_listener globalListener{globalAdded, globalRemoved};
  static void idled(void* data, ext_idle_notification_v1* object);
  static void resumed(void* data, ext_idle_notification_v1* object);
  const struct ext_idle_notification_v1_listener idleListener{idled, resumed};
  // Choose get_idle_notification (v1, respects inhibitors) vs
  // get_input_idle_notification (v2, input-only) based on the mode and the bound
  // notifier version. Called from the constructor after binding and from
  // setIdleDetection() on a mode change.
  void chooseGetIdleNotification();
  // Re-create the idle notification on a setIdleDetection() change. The old request is
  // kept alive during the switch; see the pattern comment on the members below.
  void recreateNotification();
  wl_seat* seat;
  ext_idle_notifier_v1* idleNotifier = nullptr;
  // Re-creation pattern: an ext_idle_notification_v1 event (idled/resumed) belongs to
  // the request that fired it, and its timeout is fixed at creation. Re-creating the
  // request (on a minIdleTime/mode change) would drop a pending idle->resumed
  // transition, so while we think the user is idle we keep the request that already
  // fired idled alive until it either reports resumed (user became active) or the new
  // request confirms idle again. idleNotification is always the request armed with the
  // latest settings; pendingResumeNotification is the kept one awaiting a resume.
  ext_idle_notification_v1* idleNotification = nullptr;
  // Kept request that already fired idled; only fires resumed from here on.
  ext_idle_notification_v1* pendingResumeNotification = nullptr;

  struct ext_idle_notification_v1* (*get_idle_notification)(
      struct ext_idle_notifier_v1* ext_idle_notifier_v1, uint32_t timeout,
      struct wl_seat* seat);

  bool isWatching = false;
};
