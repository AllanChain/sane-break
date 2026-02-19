// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>

#include "core/app-states.h"
#include "core/break-windows.h"
#include "core/db.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/meeting-prompt.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"

struct AppDependencies {
  SanePreferences* preferences = nullptr;
  BreakDatabase* db = nullptr;
  AbstractTimer* countDownTimer = nullptr;
  AbstractTimer* screenLockTimer = nullptr;
  SystemIdleTime* idleTimer = nullptr;
  AbstractSystemMonitor* systemMonitor = nullptr;
  AbstractBreakWindows* breakWindows = nullptr;
  AbstractMeetingPrompt* meetingPrompt = nullptr;
};

struct TrayData {
  bool isBreaking;
  int secondsToNextBreak;
  int secondsToNextBigBreak;
  int secondsFromLastBreakToNext;
  int smallBreaksBeforeBigBreak;
  PauseReasons pauseReasons;
  bool isInMeeting;
  int meetingSecondsRemaining;
  int meetingTotalSeconds;
  bool isPostponing;
};

class AbstractApp : public AppContext {
  Q_OBJECT
 public:
  AbstractApp(const AppDependencies& deps, QObject* parent = nullptr);
  ~AbstractApp() = default;

  virtual void start();

  void breakNow();
  void bigBreakNow();
  // Take a small break when big break is on
  void smallBreakInstead();
  void postpone(int seconds);
  void enableBreak();

  void startMeeting(int seconds, const QString& reason);
  void endMeetingBreakLater(int seconds = 0);
  void extendMeeting(int seconds);

 signals:
  void trayDataUpdated(TrayData);

 protected:
  AbstractTimer* m_countDownTimer;
  AbstractSystemMonitor* m_systemMonitor;

  void onBatterySettingChange();
  void onExit();
  void updateTray();

  virtual void doLockScreen() = 0;
};
