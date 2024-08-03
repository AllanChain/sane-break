// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_TRAY_H
#define SANE_TRAY_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include "idle-time.h"
#include "pref-window.h"
#include "window-manager.h"

class SaneBreakApp : public QObject {
  Q_OBJECT
 public:
  SaneBreakApp();
  ~SaneBreakApp();
  void start();
  void breakNow();
  void postpone(int secs);
  void pauseBreak();
  void resumeBreak();
  void resetBreak();
  int breakTime();
  int smallBreaksBeforeBig();

 signals:
  void quit();

 private:
  PreferenceWindow *prefWindow;
  BreakWindowManager *breakManager;
  SystemIdleTime *idleTimer;
  SleepMonitor *sleepMonitor;
  QSystemTrayIcon *icon;
  QTimer *countDownTimer;
  QMenu *menu;
  QAction *nextBreakAction;
  QAction *bigBreakAction;
  int breakCycleCount = 1;
  void createMenu();
  void tick();
  void updateMenu();
  int secondsToNextBreak;
  bool inPause = false;
};

#endif  // SANE_TRAY_H
