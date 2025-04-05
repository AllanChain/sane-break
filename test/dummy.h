// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_TEST_DUMMY_H
#define SANE_TEST_DUMMY_H
#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QSettings>
#include <QTemporaryFile>
#include <QTest>

#include "lib/app-core.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"

class DummyIdleTime : public SystemIdleTime {
 public:
  using SystemIdleTime::SystemIdleTime;
  void startWatching() {};
  void stopWatching() {};
  void setWatchAccuracy(int accuracy) {};
  void setMinIdleTime(int idleTime) {};
};

class DummyApp : public AbstractApp {
 public:
  DummyApp(AppDependencies deps, QObject* parent = nullptr)
      : AbstractApp(deps, parent) {
    connect(this, &DummyApp::trayDataUpdated, this,
            [this](TrayData data) { trayData = data; });
  };
  MOCK_METHOD(void, openBreakWindow, (bool), (override));
  MOCK_METHOD(void, closeBreakWindow, (), (override));
  MOCK_METHOD(void, mayLockScreen, (), (override));
  void advance(int secs) {
    QVERIFY2(m_countDownTimer->isActive(),
             "Impossible to simulate timer tick with an inactive timer");
    for (int i = 0; i < secs; i++) tick();
  }
  TrayData trayData;
  static AppDependencies makeDeps() {
    QTemporaryFile tempFile;
    return {.countDownTimer = new ITimer(),
            .oneshotIdleTimer = new DummyIdleTime(),
            .screenLockTimer = new ITimer(),
            .preferences = new SanePreferences(
                new QSettings(tempFile.fileName(), QSettings::IniFormat))};
  };
};

#endif  // !SANE_TEST_DUMMY_H
