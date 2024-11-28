// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_TRAY_H
#define SANE_TRAY_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include "battery-status.h"
#include "idle-time.h"
#include "pref-window.h"
#include "program-monitor.h"
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
  void pauseBreak(uint reason);
  bool resumeBreak(uint reason);
  int smallBreaksBeforeBig();
  void onSleepEnd();
  void onBreakEnd();
  void onIdleStart();
  void onIdleEnd();
  void onOneshotIdleEnd();
  void onBattery();
  void onPower();
  void onBatterySettingChange();

 signals:
  void quit();

 private:
  PreferenceWindow *prefWindow;
  BreakWindowManager *breakManager;
  SystemIdleTime *idleTimer;
  SystemIdleTime *oneshotIdleTimer;
  SleepMonitor *sleepMonitor;
  BatteryStatus *batteryWatcher;
  QSystemTrayIcon *icon;
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
  uint pauseReasons = 0;
  RunningProgramsMonitor *runningProgramsMonitor;
};

#endif  // SANE_TRAY_H
