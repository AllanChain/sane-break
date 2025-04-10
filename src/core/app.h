// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>

#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"
#include "core/window-control.h"

struct AppDependencies {
  SanePreferences *preferences = nullptr;
  AbstractTimer *countDownTimer = nullptr;
  SystemIdleTime *oneshotIdleTimer = nullptr;
  AbstractTimer *screenLockTimer = nullptr;
  AbstractSystemMonitor *systemMonitor = nullptr;
  AbstractWindowControl *windowControl = nullptr;
};

struct TrayData {
  bool isBreaking;
  int secondsToNextBreak;
  int secondsToNextBigBreak;
  int secondsFromLastBreakToNext;
  int smallBreaksBeforeBigBreak;
  SaneBreak::PauseReasons pauseReasons;
};

class AbstractApp : public QObject {
  Q_OBJECT
 public:
  AbstractApp(const AppDependencies &deps, QObject *parent = nullptr);
  ~AbstractApp() = default;

  virtual void start();
  virtual void doLockScreen() = 0;

  void breakNow();
  void bigBreakNow();
  // Take a small break when big break is on
  void smallBreakInstead();
  void postpone(int secs);
  void pauseBreak(SaneBreak::PauseReasons reason);
  void resumeBreak(SaneBreak::PauseReasons reason);
  void enableBreak();

 signals:
  void trayDataUpdated(TrayData trayData);

 protected:
  bool m_isBreaking = false;
  int m_breakCycleCount = 1;
  int m_secondsPaused = 0;
  int m_secondsToNextBreak;
  SaneBreak::PauseReasons m_pauseReasons = {};

  SanePreferences *preferences;
  AbstractSystemMonitor *m_systemMonitor;
  AbstractTimer *m_countDownTimer;
  SystemIdleTime *m_oneshotIdleTimer;
  AbstractTimer *m_screenLockTimer;
  AbstractWindowControl *m_windowControl;

  void tick();
  void onSleepEnd();
  void onBreakCountDownStateChange(bool countingDown);
  void onBreakAbort();
  void onBreakEnd();
  void onIdleStart();
  void onIdleEnd();
  void onOneshotIdleEnd();
  void onBattery();
  void onPower();
  void onBatterySettingChange();

  int smallBreaksBeforeBig();
  void updateTray();
  void resetSecondsToNextBreak();
};
