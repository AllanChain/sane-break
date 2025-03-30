// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_APP_H
#define SANE_APP_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include "gui/pref-window.h"
#include "gui/tray.h"
#include "gui/window-manager.h"
#include "lib/battery-status.h"
#include "lib/idle-time.h"
#include "lib/program-monitor.h"

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
  void onPostponeMinutesChange();
  void mayLockScreen();
  void confirmQuit();

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
  QMenu *postponeMenu;
  QAction *quitAction;
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
