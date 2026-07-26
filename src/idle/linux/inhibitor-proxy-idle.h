// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "core/idle-time.h"

class LinuxIdleInhibitor;

// Wraps an event-driven SystemIdleTime (e.g. the Wayland plugin) with logind
// inhibitor monitoring.
//
// This exists as a wrapper rather than inlined into IdleTimeWayland because the
// Wayland plugin is a dynamically-loaded .so that links only sane-core-idle,
// while LinuxIdleInhibitor lives in sane-idle (the static lib that loads the plugin).
// Wiring them at the factory composition root avoids dragging Qt DBus into the
// plugin or creating a circular dependency.
//
// ext-idle-notify v1 respects Wayland-protocol inhibitors (zwp_idle_inhibitor_v1)
// but not logind D-Bus inhibitors, so some apps' inhibitors are invisible to it.
// This wrapper gates idleStart on the inhibitor state and re-emits on inhibitor release
// (the compositor won't re-fire idled() for an already-idle session).
class InhibitorProxyIdleTime : public SystemIdleTime {
  Q_OBJECT
 public:
  InhibitorProxyIdleTime(SystemIdleTime* source, LinuxIdleInhibitor* inhibitor,
                         QObject* parent = nullptr);
  void startWatching() override;
  void stopWatching() override;
  void setWatchAccuracy(int accuracy) override;
  void setMinIdleTime(int idleTime) override;
  void setIdleMode(IdleMode mode) override;
  bool isInhibited() const override;

 private slots:
  void onSourceIdleStart();
  void onSourceIdleEnd();
  void onInhibitorChanged();

 private:
  SystemIdleTime* m_source;
  LinuxIdleInhibitor* m_inhibitor;
  void reevaluate();
};
