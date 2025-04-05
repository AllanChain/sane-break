// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QTest>
#include <QTimer>

#include "dummy.h"
#include "lib/app-core.h"
#include "lib/flags.h"

using testing::Mock;
using testing::NiceMock;

class TestApp : public QObject {
  Q_OBJECT
 private slots:
  void init() { QTest::failOnWarning(); }
  void appInitialState() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    QCOMPARE(app.trayData.pauseReasons.toInt(), 0);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  void tick() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    QVERIFY(deps.countDownTimer->isActive());

    emit deps.countDownTimer->timeout();
    // Time has ellapsed
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get() - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void showFirstBreak() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    // Break window will show
    EXPECT_CALL(app, openBreakWindow(false)).Times(1);
    app.advance(app.trayData.secondsToNextBreak);
    // Correctly advaces to next break
    QCOMPARE(app.trayData.secondsToNextBreak, 0);

    // Countdown is stopped
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, 0);

    QVERIFY(Mock::VerifyAndClearExpectations(&app));

    // Simulate break window close
    app.onBreakEnd();

    // The remaining time is correct
    int smallEvery = deps.preferences->smallEvery->get();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    // Countdown is stopped
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  void showBigBreak() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int numberOfBreaks = deps.preferences->bigAfter->get();
    EXPECT_CALL(app, openBreakWindow(false)).Times(numberOfBreaks - 1);
    EXPECT_CALL(app, openBreakWindow(true)).Times(1);
    for (int j = 0; j < numberOfBreaks; j++) {
      QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, numberOfBreaks - j - 1);
      app.advance(app.trayData.secondsToNextBreak);
      app.onBreakEnd();
    }
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void idleAfterBreak() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    deps.oneshotIdleTimer->isIdle = true;
    emit deps.oneshotIdleTimer->idleStart();
    app.onBreakEnd();

    // The remaining time is correct
    int smallEvery = deps.preferences->smallEvery->get();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    // Countdown is paused if user is idle after break
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    deps.oneshotIdleTimer->isIdle = false;
    emit deps.oneshotIdleTimer->idleEnd();

    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  void notIdleAfterBreak() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    deps.oneshotIdleTimer->isIdle = true;
    emit deps.oneshotIdleTimer->idleStart();
    deps.oneshotIdleTimer->isIdle = false;
    emit deps.oneshotIdleTimer->idleEnd();

    app.onBreakEnd();

    // Countdown resumed
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  void lockScreenTimerRunning() {
    AppDependencies deps = DummyApp::makeDeps();
    int autoScreenLockSeconds = 20;
    deps.preferences->autoScreenLock->set(autoScreenLockSeconds);

    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    EXPECT_CALL(app, mayLockScreen()).Times(0);

    // Emitted by window manager when user is idle or force break
    app.onBreakResume();

    QVERIFY(deps.screenLockTimer->isActive());
    QVERIFY(deps.screenLockTimer->isSingleShot());

    app.onBreakEnd();

    EXPECT_CALL(app, mayLockScreen()).Times(1);
    emit deps.screenLockTimer->timeout();

    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void lockScreenTimerNotRunIfNotConfigured() {
    AppDependencies deps = DummyApp::makeDeps();
    int autoScreenLockSeconds = 0;
    deps.preferences->autoScreenLock->set(autoScreenLockSeconds);

    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    EXPECT_CALL(app, mayLockScreen()).Times(0);
    app.onBreakResume();
    QVERIFY(!deps.screenLockTimer->isActive());
    app.onBreakEnd();
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void postponeTime() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.postpone(100);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak + 100);
  }
  void pauseBreakOnIdle() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.onIdleStart();
    QCOMPARE(app.trayData.pauseReasons, SaneBreak::PauseReason::Idle);
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    app.onIdleEnd();
    QVERIFY(!app.trayData.pauseReasons);
    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  void pauseBreakOnBattery() {
    AppDependencies deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.onBattery();
    QCOMPARE(app.trayData.pauseReasons, SaneBreak::PauseReason::OnBattery);
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    app.onPower();
    QVERIFY(!app.trayData.pauseReasons);
    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  void resetAfterPause() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(app.trayData.secondsToNextBreak);
    app.onBreakEnd();
    app.advance(100);

    int smallBreaksBeforeBigBreak = app.trayData.smallBreaksBeforeBigBreak;
    app.onIdleStart();
    app.advance(deps.preferences->resetAfterPause->get() + 1);
    app.onIdleEnd();

    int smallEvery = deps.preferences->smallEvery->get();
    // Countdown resumed and the time has been reset
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, smallBreaksBeforeBigBreak);
  }
  void resetCycleAfterPause() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(100);
    app.onIdleStart();
    app.advance(deps.preferences->resetCycleAfterPause->get() + 1);
    app.onIdleEnd();

    int smallEvery = deps.preferences->smallEvery->get();
    // Countdown resumed and the time has been reset
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
};

QTEST_APPLESS_MAIN(TestApp)
#include "test-app.moc"
