// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QFlags>
#include <QObject>

#include "lib/flags.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"

class ITimer : public QObject {
  Q_OBJECT
 public:
  ITimer(QObject *parent = nullptr) : QObject(parent) {};
  ~ITimer() = default;
  virtual void start() { m_active = true; };
  virtual void start(int msec) { m_active = m_interval = msec; };
  virtual void stop() { m_active = false; };
  virtual bool isActive() { return m_active; }
  virtual void setSingleShot(bool singleShot) { m_singleShot = singleShot; };
  virtual bool isSingleShot() { return m_singleShot; };
  virtual void setInterval(int msec) { m_interval = msec; };
  virtual int interval() { return m_interval; }
 signals:
  void timeout();

 private:
  bool m_singleShot = false;
  bool m_interval = 0;
  bool m_active = false;
};

// Minimal dependencies
struct AppDependencies {
  ITimer *countDownTimer = nullptr;
  SystemIdleTime *oneshotIdleTimer = nullptr;
  ITimer *screenLockTimer = nullptr;
  SanePreferences *preferences = nullptr;
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
  virtual void openBreakWindow(bool isBigBreak) = 0;
  virtual void closeBreakWindow() = 0;
  virtual void mayLockScreen() = 0;

  void tick();
  void breakNow();
  void bigBreakNow();
  void postpone(int secs);
  void pauseBreak(SaneBreak::PauseReasons reason);
  void resumeBreak(SaneBreak::PauseReasons reason);
  void enableBreak();

  void onSleepEnd();
  void onBreakResume();
  void onBreakEnd();
  void onIdleStart();
  void onIdleEnd();
  void onOneshotIdleEnd();
  void onBattery();
  void onPower();

 signals:
  void trayDataUpdated(TrayData trayData);

 protected:
  bool m_isBreaking = false;
  int m_breakCycleCount = 1;
  int m_secondsPaused = 0;
  int m_secondsToNextBreak;
  SaneBreak::PauseReasons m_pauseReasons = {};

  SanePreferences *preferences;
  ITimer *m_countDownTimer;
  SystemIdleTime *m_oneshotIdleTimer;
  ITimer *m_screenLockTimer;

  int smallBreaksBeforeBig();
  void updateTray();
  void resetSecondsToNextBreak();
};
