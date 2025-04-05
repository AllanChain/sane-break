// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QSettings>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>

#include "lib/app-core.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"

using testing::Mock;
using testing::NiceMock;

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
    for (int i = 0; i < secs; i++) tick();
  }
  TrayData trayData;
};

class TestApp : public QObject {
  Q_OBJECT
 private:
  AppDependencies makeDeps() {
    QTemporaryFile tempFile;
    return {.countDownTimer = new ITimer(),
            .oneshotIdleTimer = new DummyIdleTime(),
            .screenLockTimer = new ITimer(),
            .preferences = new SanePreferences(
                new QSettings(tempFile.fileName(), QSettings::IniFormat))};
  };

 private slots:
  void init() { QTest::failOnWarning(); }
  void appInitialState() {
    AppDependencies deps = makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    QCOMPARE(app.trayData.pauseReasons.toInt(), 0);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  void tick() {
    AppDependencies deps = makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    QVERIFY(deps.countDownTimer->isActive());

    emit deps.countDownTimer->timeout();
    // Time has ellapsed
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get() - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void showFirstBreak() {
    AppDependencies deps = makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    // Break window will show
    EXPECT_CALL(app, openBreakWindow(false)).Times(1);
    app.advance(app.trayData.secondsToNextBreak);
    // Correctly advaces to next break
    QCOMPARE(app.trayData.secondsToNextBreak, 0);
    // Countdown is stopped
    QVERIFY(!deps.countDownTimer->isActive());
    QVERIFY(Mock::VerifyAndClearExpectations(&app));

    // Simulate break window close
    app.onBreakEnd();

    // Countdown is running again
    QVERIFY(deps.countDownTimer->isActive());
    // The remaining time is correct
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  void showBigBreak() {
    AppDependencies deps = makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int numberOfBreaks = deps.preferences->bigAfter->get();
    EXPECT_CALL(app, openBreakWindow(false)).Times(numberOfBreaks - 1);
    EXPECT_CALL(app, openBreakWindow(true)).Times(1);
    for (int j = 0; j < numberOfBreaks; j++) {
      app.advance(app.trayData.secondsToNextBreak);
      app.onBreakEnd();
    }
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void idleAfterBreak() {
    AppDependencies deps = makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    deps.oneshotIdleTimer->isIdle = true;
    emit deps.oneshotIdleTimer->idleStart();
    app.onBreakEnd();

    // Countdown is paused if user is idle after break
    QVERIFY(!deps.countDownTimer->isActive());

    deps.oneshotIdleTimer->isIdle = false;
    emit deps.oneshotIdleTimer->idleEnd();

    // Countdown resumed
    QVERIFY(deps.countDownTimer->isActive());
  }
  void notIdleAfterBreak() {
    AppDependencies deps = makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    deps.oneshotIdleTimer->isIdle = true;
    emit deps.oneshotIdleTimer->idleStart();
    deps.oneshotIdleTimer->isIdle = false;
    emit deps.oneshotIdleTimer->idleEnd();

    app.onBreakEnd();

    // Countdown resumed
    QVERIFY(deps.countDownTimer->isActive());
  }
};

QTEST_APPLESS_MAIN(TestApp)
#include "test-app.moc"
