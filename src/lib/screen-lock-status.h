// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

// Observes whether the user session is locked and emits on each transition. Mirrors
// BatteryStatus: the platform subclass answers systemScreenLocked(), this base owns the
// poll and the signals.
//
// The polled read is the only source of truth. Platform lock events (logind on Linux,
// WM_WTSSESSION_CHANGE on Windows, loginwindow's distributed notifications on macOS)
// only trigger a check(), so a transition is seen now rather than up to watchAccuracy
// later. They are never taken as state: an unlock event can go missing on all three,
// and trusting it would strand a paused schedule for good.
//
// Best-effort by design - a session that cannot report lock state answers "not locked",
// leaving the idle pause as the only absence detector. Guessing a lock would drop break
// reminders with nothing to explain them.
class ScreenLockStatus : public QObject {
  Q_OBJECT
 public:
  ScreenLockStatus(QObject* parent = nullptr);
  void startWatching();
  void stopWatching();
  virtual bool systemScreenLocked() { return false; }
  static ScreenLockStatus* createWatcher(QObject* parent = nullptr);
  // Poll interval in ms. 5 s stays unnoticeable next to the idle pause (pauseOnIdleFor,
  // default 3 min) and is cheap enough to run forever.
  int watchAccuracy = 5000;

 public slots:
  // Re-reads the state, emitting only on transition. The poll timer and the platform
  // notification handlers both call this.
  void check();

 signals:
  void screenLocked();
  void screenUnlocked();

 private:
  QTimer* timer;
  // What check() last saw; signals fire only when it changes.
  bool isLocked = false;
};
