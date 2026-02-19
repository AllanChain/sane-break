// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QSettings>
#include <QSqlDatabase>
#include <QTemporaryFile>
#include <QTest>
#include <memory>

#include "core/app-states.h"
#include "core/app.h"
#include "core/break-windows.h"
#include "core/db.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/meeting-prompt.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"
#include "gmock/gmock.h"

inline SanePreferences* tempPreferences() {
  QTemporaryFile tempFile;
  (void)tempFile.open();
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
  MOCK_METHOD(void, create, (BreakType, SanePreferences*), (override));
  MOCK_METHOD(void, destroy, (), ());
  MOCK_METHOD(void, setTime, (int), ());
  MOCK_METHOD(void, showFullScreen, (), ());
  MOCK_METHOD(void, showFlashPrompt, (), ());
  MOCK_METHOD(void, showButtons, (Buttons, bool), ());
  MOCK_METHOD(void, playEnterSound, (BreakType, SanePreferences*), ());
  MOCK_METHOD(void, playExitSound, (BreakType, SanePreferences*), ());
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

class DummyMeetingPrompt : public AbstractMeetingPrompt {
  Q_OBJECT
 public:
  explicit DummyMeetingPrompt(QObject* parent = nullptr)
      : AbstractMeetingPrompt(parent) {
    ON_CALL(*this, showEndPrompt()).WillByDefault([this]() { m_showing = true; });
    ON_CALL(*this, closeEndPrompt()).WillByDefault([this]() { m_showing = false; });
  }
  MOCK_METHOD(void, showEndPrompt, (), (override));
  MOCK_METHOD(void, closeEndPrompt, (), (override));
  MOCK_METHOD(void, resetTimeout, (), (override));
  bool isShowing() const override { return m_showing; }
  bool m_showing = false;
};

struct DummyAppDependencies {
  SanePreferences* preferences = nullptr;
  BreakDatabase* db = nullptr;
  AbstractTimer* countDownTimer = nullptr;
  AbstractTimer* screenLockTimer = nullptr;
  DummyIdleTime* idleTimer = nullptr;
  DummySystemMonitor* systemMonitor = nullptr;
  DummyBreakWindows* breakWindows = nullptr;
  DummyMeetingPrompt* meetingPrompt = nullptr;
};
class DummyApp : public AbstractApp {
  Q_OBJECT
 public:
  DummyApp(const DummyAppDependencies& deps, QObject* parent = nullptr)
      : AbstractApp(
            {deps.preferences, deps.db, deps.countDownTimer, deps.screenLockTimer,
             deps.idleTimer, deps.systemMonitor, deps.breakWindows, deps.meetingPrompt},
            parent) {
    connect(this, &DummyApp::trayDataUpdated, this,
            [this](TrayData data) { trayData = data; });
  };
  MOCK_METHOD(void, doLockScreen, (), (override));
  AppState::StateID currentState() { return m_currentState->getID(); }
  void advance(int secs) {
    QVERIFY2(m_countDownTimer->isActive(),
             "Impossible to simulate timer tick with an inactive timer");
    for (int i = 0; i < secs; i++) tick();
  }
  void advanceToBreakEnd() {
    while (m_currentState->getID() == AppState::Break) tick();
  }
  void advanceToForceBreakStart() {
    QCOMPARE(m_currentState->getID(), AppState::Break);
    auto* breakState = dynamic_cast<AppStateBreak*>(m_currentState.get());
    while (!breakState->data->isForceBreak()) tick();
  }
  TrayData trayData;
  static DummyAppDependencies makeDeps(QObject* parent = nullptr) {
    for (auto& conn : QSqlDatabase::connectionNames()) {
      QSqlDatabase::removeDatabase(conn);
    }
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    return {
        .preferences = tempPreferences(),
        .db = new BreakDatabase(db, parent),
        .countDownTimer = new AbstractTimer(parent),
        .screenLockTimer = new AbstractTimer(parent),
        .idleTimer = new DummyIdleTime(parent),
        .systemMonitor = new DummySystemMonitor(parent),
        .breakWindows = new testing::NiceMock<DummyBreakWindows>(parent),
        .meetingPrompt = new testing::NiceMock<DummyMeetingPrompt>(parent),
    };
  };
};
