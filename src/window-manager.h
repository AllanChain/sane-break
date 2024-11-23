// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_WINDOW_MANAGER_H
#define SANE_BREAK_WINDOW_MANAGER_H

#include <QList>
#include <QObject>

#include "break-window.h"
#include "idle-time.h"

class BreakWindowManager : public QObject {
  Q_OBJECT

 public:
  BreakWindowManager();
  ~BreakWindowManager();
  int remainingTime;
  void show(BreakType type);
  bool isShowing();
  void close();

 signals:
  void timeout();

 private:
  bool isIdle = false;
  bool isForceBreak = false;
  int totalTime;
  QList<BreakWindow *> windows;
  QTimer *countdownTimer;
  QTimer *forceBreakTimer;
  SystemIdleTime *idleTimer;
  void createWindows(BreakType type);
  void tick();
  void forceBreak();
  void onIdleStart();
  void onIdleEnd();
  void playSound(QUrl url);
};

#endif  // SANE_BREAK_WINDOW_MANAGER_H
