// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QTest>
#include <QTimer>

#include "core/app.h"
#include "core/flags.h"
#include "dummy.h"

using testing::Mock, testing::Return, testing::NiceMock;

class TestApp : public QObject {
  Q_OBJECT
 private slots:
  void init() { QTest::failOnWarning(); }
  void app_initial_state() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    QCOMPARE(app.trayData.pauseReasons.toInt(), 0);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  void tick() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    QVERIFY(deps.countDownTimer->isActive());

    emit deps.countDownTimer->timeout();
    // Time has ellapsed
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get() - 1);
  }
  void show_first_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    // Break window will show
    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Small))
        .Times(1);
    app.advance(app.trayData.secondsToNextBreak);
    // Correctly advaces to next break
    QCOMPARE(app.trayData.secondsToNextBreak, 0);

    // Countdown is stopped
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, 0);

    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));

    // Simulate break window close
    deps.windowControl->advanceToEnd();

    // The remaining time is correct
    int smallEvery = deps.preferences->smallEvery->get();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    // Countdown is stopped
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  void break_tick() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(app.trayData.secondsToNextBreak);
    QCOMPARE(app.trayData.secondsToNextBreak, 0);

    QVERIFY(app.trayData.isBreaking);
    int expectedTimeForBreak =
        deps.preferences->smallFor->get() + deps.preferences->flashFor->get();
    app.advance(expectedTimeForBreak);
    QVERIFY(!app.trayData.isBreaking);

    // The remaining time is correct
    int smallEvery = deps.preferences->smallEvery->get();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);
  }
  void show_big_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int numberOfBreaks = deps.preferences->bigAfter->get();
    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Small))
        .Times(numberOfBreaks - 1);
    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Big)).Times(1);
    for (int j = 0; j < numberOfBreaks; j++) {
      QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, numberOfBreaks - j - 1);
      app.advance(app.trayData.secondsToNextBreak);
      deps.windowControl->advanceToEnd();
    }
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));
  }
  void countdown_pause_if_idle_after_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    deps.oneshotIdleTimer->setIdle(true);
    deps.windowControl->advanceToEnd();

    // The remaining time is correct
    int smallEvery = deps.preferences->smallEvery->get();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    // Countdown is paused if user is idle after break
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    deps.oneshotIdleTimer->setIdle(false);

    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  void countdown_resume_if_not_idle_after_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    deps.oneshotIdleTimer->setIdle(true);
    deps.oneshotIdleTimer->setIdle(false);

    deps.windowControl->advanceToEnd();

    // Countdown resumed
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  void lock_screen_timer_running() {
    auto deps = DummyApp::makeDeps();
    int autoScreenLockSeconds = 20;
    deps.preferences->autoScreenLock->set(autoScreenLockSeconds);

    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    EXPECT_CALL(app, doLockScreen()).Times(0);

    // Emitted by window manager when user is idle or force break
    emit deps.windowControl->countDownStateChanged(true);

    QVERIFY(deps.screenLockTimer->isActive());
    QVERIFY(deps.screenLockTimer->isSingleShot());

    deps.windowControl->advanceToEnd();

    EXPECT_CALL(app, doLockScreen()).Times(1);
    emit deps.screenLockTimer->timeout();

    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void lock_screen_timer_not_run_if_not_configured() {
    auto deps = DummyApp::makeDeps();
    int autoScreenLockSeconds = 0;
    deps.preferences->autoScreenLock->set(autoScreenLockSeconds);

    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    EXPECT_CALL(app, doLockScreen()).Times(0);
    emit deps.windowControl->countDownStateChanged(true);
    QVERIFY(!deps.screenLockTimer->isActive());
    deps.windowControl->advanceToEnd();

    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void postpone_time() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.postpone(100);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak + 100);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);
  }
  void pause_break_on_idle() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    emit deps.systemMonitor->idleStarted();
    QCOMPARE(app.trayData.pauseReasons, SaneBreak::PauseReason::Idle);
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    emit deps.systemMonitor->idleEnded();
    QVERIFY(!app.trayData.pauseReasons);
    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  void pause_break_on_battery() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    emit deps.systemMonitor->batteryPowered();
    QCOMPARE(app.trayData.pauseReasons, SaneBreak::PauseReason::OnBattery);
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    emit deps.systemMonitor->adaptorPowered();
    QVERIFY(!app.trayData.pauseReasons);
    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  void pause_break_on_app_running() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    emit deps.systemMonitor->programStarted();
    QCOMPARE(app.trayData.pauseReasons, SaneBreak::PauseReason::AppOpen);
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    emit deps.systemMonitor->programStopped();
    QVERIFY(!app.trayData.pauseReasons);
    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  void reset_countdown_after_pause() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(app.trayData.secondsToNextBreak);
    deps.windowControl->advanceToEnd();
    app.advance(100);

    int smallBreaksBeforeBigBreak = app.trayData.smallBreaksBeforeBigBreak;
    emit deps.systemMonitor->idleStarted();
    app.advance(deps.preferences->resetAfterPause->get() + 1);
    emit deps.systemMonitor->idleEnded();

    int smallEvery = deps.preferences->smallEvery->get();
    // Countdown resumed and the time has been reset
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, smallBreaksBeforeBigBreak);

    // Subsequent short idles should not be affected
    emit deps.systemMonitor->idleStarted();
    app.advance(1);
    emit deps.systemMonitor->idleEnded();
    // Since the time ellapsed is in paused state, the time should not change
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  void reset_cycle_after_pause() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(100);
    emit deps.systemMonitor->idleStarted();
    app.advance(deps.preferences->resetCycleAfterPause->get() + 1);
    emit deps.systemMonitor->idleEnded();

    int smallEvery = deps.preferences->smallEvery->get();
    // Countdown resumed and the time has been reset
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  void take_small_break_instead() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Big)).Times(1);
    app.bigBreakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));

    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Small))
        .Times(1);
    EXPECT_CALL(*deps.windowControl, deleteWindows()).Times(1);
    app.smallBreakInstead();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));
    QVERIFY(app.trayData.isBreaking);

    deps.windowControl->advanceToEnd();
    QVERIFY(!app.trayData.isBreaking);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  void postpone_while_break() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Small))
        .Times(1);
    app.breakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));

    EXPECT_CALL(*deps.windowControl, deleteWindows()).Times(1);
    app.postpone(100);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);
    QCOMPARE(app.trayData.secondsToNextBreak, 100);
    QCOMPARE(app.trayData.secondsToNextBigBreak, 100);
  }
  void pause_while_break() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Small))
        .Times(1);
    app.breakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));

    EXPECT_CALL(*deps.windowControl, deleteWindows()).Times(1);
    emit deps.systemMonitor->batteryPowered();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));
  }
  void idle_while_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Small))
        .Times(1);
    app.breakNow();
    EXPECT_CALL(*deps.windowControl, deleteWindows()).Times(0);
    emit deps.systemMonitor->idleStarted();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));
    QVERIFY(app.trayData.isBreaking);
    QCOMPARE(app.trayData.secondsToNextBreak, 0);
  }
  void sleep_end_while_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.windowControl, createWindows(SaneBreak::BreakType::Small))
        .Times(1);
    app.breakNow();
    EXPECT_CALL(*deps.windowControl, deleteWindows()).Times(1);
    emit deps.systemMonitor->sleepEnded();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.windowControl));
    QVERIFY(!app.trayData.isBreaking);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  void more_than_one_pause_reasons() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    emit deps.systemMonitor->batteryPowered();
    emit deps.systemMonitor->idleStarted();

    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
    QVERIFY(app.trayData.pauseReasons.testFlags(SaneBreak::PauseReason::Idle |
                                                SaneBreak::PauseReason::OnBattery));

    emit deps.systemMonitor->idleEnded();
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
    QVERIFY(app.trayData.pauseReasons.testFlag(SaneBreak::PauseReason::OnBattery));

    emit deps.systemMonitor->adaptorPowered();
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
    QVERIFY(!app.trayData.pauseReasons);
  }
  void confirm_postpone_if_long_time_since_last_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallEvery = deps.preferences->smallEvery->get();
    app.advance(smallEvery - 1);

    // First postpone should not trigger confirmation
    EXPECT_CALL(app, confirmPostpone).Times(0);
    app.postpone(100);
    QCOMPARE(app.trayData.secondsToNextBreak, 101);
    app.advance(100);
    QVERIFY(Mock::VerifyAndClearExpectations(&app));

    // Second postpone triggers confirmation and the break is postponed
    EXPECT_CALL(app, confirmPostpone).WillOnce(Return(true));
    app.postpone(100);
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
    QCOMPARE(app.trayData.secondsToNextBreak, 101);
  }
  void no_confirm_postpone_if_countdown_reset() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallEvery = deps.preferences->smallEvery->get();
    app.advance(smallEvery - 1);

    emit deps.systemMonitor->sleepEnded();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);
    app.advance(smallEvery - 1);

    // Postpone should not trigger confirmation
    EXPECT_CALL(app, confirmPostpone).Times(0);
    app.postpone(100);
    QCOMPARE(app.trayData.secondsToNextBreak, 101);
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
};

QTEST_APPLESS_MAIN(TestApp)
#include "test-app.moc"
