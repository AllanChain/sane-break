// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_WINDOW_MANAGER_H
#define SANE_BREAK_WINDOW_MANAGER_H

#include <QList>
#include <QObject>
#include <QTimer>

#include "break-window.h"  // IWYU pragma: export
#include "lib/idle-time.h"
#include "sound-player.h"

class BreakWindowManager : public QObject {
  Q_OBJECT

 public:
  BreakWindowManager();
  ~BreakWindowManager();
  int remainingTime;
  BreakType currentType;
  void show(BreakType type);
  bool isShowing();
  void close();

 signals:
  void resume();
  void timeout();

 private:
  bool isIdle = false;
  bool isForceBreak = false;
  int totalTime;
  QList<BreakWindow *> windows;
  QTimer *countdownTimer;
  QTimer *forceBreakTimer;
  SystemIdleTime *idleTimer;
  SoundPlayer *soundPlayer;
  void createWindows();
  void tick();
  void forceBreak();
  void onIdleStart();
  void onIdleEnd();
};

#endif  // SANE_BREAK_WINDOW_MANAGER_H
