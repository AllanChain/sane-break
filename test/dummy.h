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
#include <utility>

#include "core/app.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"
#include "core/window-control.h"
#include "gmock/gmock.h"

inline SanePreferences *tempPreferences() {
  QTemporaryFile tempFile;
  return new SanePreferences(new QSettings(tempFile.fileName(), QSettings::IniFormat));
}

class DummyIdleTime : public SystemIdleTime {
  Q_OBJECT
 public:
  using SystemIdleTime::SystemIdleTime;
  void startWatching() {};
  void stopWatching() {};
  void setWatchAccuracy(int accuracy) {};
  void setMinIdleTime(int idleTime) {};
};

class DummyBreakWindow : public AbstractBreakWindow {
  Q_OBJECT
 public:
  using AbstractBreakWindow::AbstractBreakWindow;
  MOCK_METHOD(void, start, (), (override));
  MOCK_METHOD(void, setTime, (int), (override));
  MOCK_METHOD(void, resizeToNormal, (), (override));
  MOCK_METHOD(void, setFullScreen, (), (override));
  MOCK_METHOD(void, showKillTip, (), (override));
};

class DummyWindowControl : public AbstractWindowControl {
  Q_OBJECT
 public:
  using AbstractWindowControl::AbstractWindowControl;
  static WindowDependencies makeDeps() {
    return {.preferences = tempPreferences(), .idleTimer = new DummyIdleTime()};
  }
  MOCK_METHOD(void, createWindows, (SaneBreak::BreakType), (override));
  MOCK_METHOD(void, deleteWindows, (), (override));
  void advanceToEnd() {
    m_remainingSeconds = 0;
    close();
  }
};

class SimpleWindowControl : public AbstractWindowControl {
  Q_OBJECT
 public:
  using AbstractWindowControl::AbstractWindowControl;
  static WindowDependencies makeDeps() {
    return {.preferences = tempPreferences(), .idleTimer = new DummyIdleTime()};
  }
  void createWindows(SaneBreak::BreakType type) {
    window = new testing::NiceMock<DummyBreakWindow>(breakData(type));
    m_windows.append(window);
  }
  bool hasWindows() { return !m_windows.isEmpty() && window != nullptr; }
  void advance(int secs) {
    for (int i = 0; i < secs; i++) tick();
  }
  DummyBreakWindow *window = nullptr;
  using AbstractWindowControl::breakData;
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

class DummyApp : public AbstractApp {
  Q_OBJECT
 public:
  DummyApp(const AppDependencies &deps, QObject *parent = nullptr)
      : AbstractApp(deps, parent) {
    connect(this, &DummyApp::trayDataUpdated, this,
            [this](TrayData data) { trayData = data; });
  };
  MOCK_METHOD(void, doLockScreen, (), (override));
  void advance(int secs) {
    QVERIFY2(m_countDownTimer->isActive(),
             "Impossible to simulate timer tick with an inactive timer");
    for (int i = 0; i < secs; i++) tick();
  }
  TrayData trayData;
  static std::pair<AppDependencies, DummyWindowControl *> makeDeps() {
    auto preferences = tempPreferences();
    WindowDependencies windowDeps = {
        .preferences = preferences,
        .idleTimer = new DummyIdleTime(),
    };
    auto windowControl = new testing::NiceMock<DummyWindowControl>(windowDeps);
    AppDependencies deps = {
        .preferences = preferences,
        .countDownTimer = new AbstractTimer(),
        .oneshotIdleTimer = new DummyIdleTime(),
        .screenLockTimer = new AbstractTimer(),
        .systemMonitor = new DummySystemMonitor(),
        .windowControl = windowControl,
    };
    return {deps, windowControl};
  };
};
