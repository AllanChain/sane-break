// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_APP_H
#define SANE_APP_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include "battery-status.h"
#include "idle-time.h"
#include "pref-window.h"
#include "program-monitor.h"
#include "tray.h"
#include "window-manager.h"

enum PauseReason {
  IDLE = 1 << 0,
  ON_BATTERY = 1 << 1,
  APP_OPEN = 1 << 2,
};

class SaneBreakApp : public QObject {
  Q_OBJECT
 public:
  SaneBreakApp();
  ~SaneBreakApp();
  void start();
  void breakNow();
  void postpone(int secs);
  void pauseBreak(unsigned int reason);
  bool resumeBreak(unsigned int reason);
  int smallBreaksBeforeBig();
  void onIconTrigger();
  void onSleepEnd();
  void onBreakResume();
  void onBreakEnd();
  void onIdleStart();
  void onIdleEnd();
  void onOneshotIdleEnd();
  void onBattery();
  void onPower();
  void onBatterySettingChange();
  void mayLockScreen();

 signals:
  void quit();

 private:
  PreferenceWindow *prefWindow;
  BreakWindowManager *breakManager;
  SystemIdleTime *idleTimer;
  QTimer *screenLockTimer;
  SystemIdleTime *oneshotIdleTimer;
  SleepMonitor *sleepMonitor;
  BatteryStatus *batteryWatcher;
  StatusTrayWindow *tray;
  QTimer *countDownTimer;
  int lastPause = 0;
  QMenu *menu;
  QAction *nextBreakAction;
  QAction *bigBreakAction;
  QAction *enableBreak;
  int breakCycleCount = 1;
  void createMenu();
  void tick();
  void updateMenu();
  int secondsToNextBreak;
  void addSecondsToNextBreak(int seconds);
  void resetSecondsToNextBreak();
  void updateIcon();
  unsigned int pauseReasons = 0;
  RunningProgramsMonitor *runningProgramsMonitor;
};

#endif  // SANE_APP_H
