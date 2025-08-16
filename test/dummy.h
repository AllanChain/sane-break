// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QSettings>
#include <QTemporaryFile>
#include <QTest>
#include <memory>

#include "core/app-states.h"
#include "core/app.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"
#include "gmock/gmock.h"

inline SanePreferences *tempPreferences() {
  QTemporaryFile tempFile;
  return new SanePreferences(new QSettings(tempFile.fileName(), QSettings::IniFormat));
}

class DummyIdleTime : public SystemIdleTime {
  Q_OBJECT
 public:
  using SystemIdleTime::SystemIdleTime;
  void startWatching() { m_isIdle = false; };
  void stopWatching() {};
  void setWatchAccuracy(int) {};
  void setMinIdleTime(int) {};
  void setIdle(bool idle) {
    m_isIdle = idle;
    if (idle) {
      emit idleStart();
    } else {
      emit idleEnd();
    }
  }
};

class DummyBreakWindows : public AbstractBreakWindows {
  Q_OBJECT
 public:
  using AbstractBreakWindows::AbstractBreakWindows;
  MOCK_METHOD(void, create, (BreakType, SanePreferences *), (override));
  MOCK_METHOD(void, destroy, (), ());
  MOCK_METHOD(void, setTime, (int), ());
  MOCK_METHOD(void, showFullScreen, (), ());
  MOCK_METHOD(void, showFlashPrompt, (), ());
  MOCK_METHOD(void, showButtons, (Buttons, bool), ());
  MOCK_METHOD(void, playEnterSound, (BreakType, SanePreferences *), ());
  MOCK_METHOD(void, playExitSound, (BreakType, SanePreferences *), ());
};

class DummySystemMonitor : public AbstractSystemMonitor {
  Q_OBJECT
 public:
  using AbstractSystemMonitor::AbstractSystemMonitor;

  void start() {};
  bool isOnBattery() { return m_battery; };
  void setOnBattery(bool battery) { m_battery = battery; };

 private:
  bool m_battery = false;
};

struct DummyAppDependencies {
  SanePreferences *preferences = nullptr;
  AbstractTimer *countDownTimer = nullptr;
  AbstractTimer *screenLockTimer = nullptr;
  DummyIdleTime *idleTimer = nullptr;
  DummySystemMonitor *systemMonitor = nullptr;
  DummyBreakWindows *breakWindows = nullptr;
};
class DummyApp : public AbstractApp {
  Q_OBJECT
 public:
  DummyApp(const DummyAppDependencies &deps, QObject *parent = nullptr)
      : AbstractApp({deps.preferences, deps.countDownTimer, deps.screenLockTimer,
                     deps.idleTimer, deps.systemMonitor, deps.breakWindows},
                    parent) {
    connect(this, &DummyApp::trayDataUpdated, this,
            [this](TrayData data) { trayData = data; });
  };
  MOCK_METHOD(void, doLockScreen, (), (override));
  MOCK_METHOD(bool, confirmPostpone, (int), (override));
  AppState::StateID currentState() { return m_currentState->getID(); }
  void advance(int secs) {
    QVERIFY2(m_countDownTimer->isActive(),
             "Impossible to simulate timer tick with an inactive timer");
    for (int i = 0; i < secs; i++) tick();
  }
  void advanceToBreakEnd() {
    QCOMPARE(currentState(), AppState::Break);
    while (data->breaks->remainingSeconds() > 0) tick();
  }
  void advanceToForceBreakStart() {
    QCOMPARE(currentState(), AppState::Break);
    while (!data->breaks->isForceBreak()) tick();
  }
  TrayData trayData;
  static DummyAppDependencies makeDeps() {
    return {
        .preferences = tempPreferences(),
        .countDownTimer = new AbstractTimer(),
        .screenLockTimer = new AbstractTimer(),
        .idleTimer = new DummyIdleTime(),
        .systemMonitor = new DummySystemMonitor(),
        .breakWindows = new testing::NiceMock<DummyBreakWindows>(),
    };
  };
};
