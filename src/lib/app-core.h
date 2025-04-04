// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_STATE_H
#define SANE_STATE_H

#include <QObject>

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

class AbstractApp : public QObject {
  Q_OBJECT
 public:
  AbstractApp(const AppDependencies &deps, QObject *parent = nullptr);
  ~AbstractApp() = default;

  enum PauseReason {
    IDLE = 1 << 0,
    ON_BATTERY = 1 << 1,
    APP_OPEN = 1 << 2,
  };
  Q_DECLARE_FLAGS(PauseReasons, PauseReason);

  virtual void start();
  virtual void openBreakWindow(bool isBigBreak) = 0;
  virtual void closeBreakWindow() = 0;
  virtual void updateTray() = 0;
  virtual void mayLockScreen() = 0;

  void postpone(int secs);
  void pauseBreak(PauseReasons reason);
  void resumeBreak(PauseReasons reason);
  int smallBreaksBeforeBig();

  void onSleepEnd();
  void onBreakResume();
  void onBreakEnd();
  void onIdleStart();
  void onIdleEnd();
  void onOneshotIdleEnd();
  void onBattery();
  void onPower();

  bool isBreaking = false;
  int breakCycleCount = 1;
  int secondsPaused = 0;
  int secondsToNextBreak;
  PauseReasons pauseReasons = {};

 protected:
  SanePreferences *preferences;
  ITimer *m_countDownTimer;
  SystemIdleTime *m_oneshotIdleTimer;
  ITimer *m_screenLockTimer;

  void tick();
  void breakNow();
  void resetSecondsToNextBreak();
};

#endif  // SANE_STATE_H
