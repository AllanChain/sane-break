// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>

#include "core/app-states.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"

struct AppDependencies {
  SanePreferences *preferences = nullptr;
  AbstractTimer *countDownTimer = nullptr;
  AbstractTimer *screenLockTimer = nullptr;
  SystemIdleTime *idleTimer = nullptr;
  AbstractSystemMonitor *systemMonitor = nullptr;
  AbstractBreakWindows *breakWindows = nullptr;
};

struct TrayData {
  bool isBreaking;
  int secondsToNextBreak;
  int secondsToNextBigBreak;
  int secondsFromLastBreakToNext;
  int smallBreaksBeforeBigBreak;
  SaneBreak::PauseReasons pauseReasons;
};

class AbstractApp : public AppContext {
  Q_OBJECT
 public:
  AbstractApp(const AppDependencies &deps, QObject *parent = nullptr);
  ~AbstractApp() = default;

  virtual void start();

  void breakNow();
  void bigBreakNow();
  // Take a small break when big break is on
  void smallBreakInstead();
  void postpone(int secs);
  void enableBreak();

 signals:
  void trayDataUpdated(TrayData);

 protected:
  AbstractTimer *m_countDownTimer;
  AbstractSystemMonitor *m_systemMonitor;

  void onBatterySettingChange();
  void onSleepEnd();
  void updateTray();

  virtual void doLockScreen() = 0;
  virtual bool confirmPostpone(int secondsToPostpone) = 0;
};
