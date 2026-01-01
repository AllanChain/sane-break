// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QTest>
#include <QTimer>

#include "core/app.h"
#include "core/flags.h"
#include "dummy.h"
#include "gmock/gmock.h"

using testing::Mock, testing::Return, testing::NiceMock, testing::_;

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
  // Simple test if the break window shows after the countdown ends.
  void show_first_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    // Break window will show
    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.advance(app.trayData.secondsToNextBreak);
    // Correctly advaces to next break
    QCOMPARE(app.trayData.secondsToNextBreak, 0);

    // Countdown is stopped
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, 0);

    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));

    // Simulate break window close
    app.advanceToBreakEnd();

    // The remaining time is correct
    int smallEvery = deps.preferences->smallEvery->get();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    // Countdown is stopped
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  /* For consistency and easy testing, the timer for the break windows is actually
   * managed by the app. Here we test if advancing the app timer is currectly
   * controlling the break windows. We are not emitting idle events and we rely on the
   * force break mechanism.
   */
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
  // Advance a full cycle of breaks and see if small and big breaks are shown correctly
  void show_big_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int numberOfBreaks = deps.preferences->bigAfter->get();
    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _))
        .Times(numberOfBreaks - 1);
    EXPECT_CALL(*deps.breakWindows, create(BreakType::Big, _)).Times(1);
    for (int j = 0; j < numberOfBreaks; j++) {
      QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, numberOfBreaks - j - 1);
      app.advance(app.trayData.secondsToNextBreak);
      app.advanceToBreakEnd();
    }
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
  }
  /* If the user is idle after the break finishes, we should not start the countdown
   * until the user comes back. This is important because if we don't do so, we are kind
   * of telling the user to engage in work immediately after the break since the clock
   * is ticking.
   */
  void countdown_pause_if_idle_after_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);
    deps.idleTimer->setIdle(true);
    app.advanceToBreakEnd();

    // The remaining time is correct
    int smallEvery = deps.preferences->smallEvery->get();
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    // Countdown is paused if user is idle after break
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);

    deps.idleTimer->setIdle(false);

    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  // See `countdown_pause_if_idle_after_break`
  void countdown_resume_if_not_idle_after_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    deps.idleTimer->setIdle(true);
    deps.idleTimer->setIdle(false);

    app.advanceToBreakEnd();

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
    app.advanceToForceBreakStart();

    QVERIFY(deps.screenLockTimer->isActive());
    QVERIFY(deps.screenLockTimer->isSingleShot());

    app.advanceToBreakEnd();

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
    app.advanceToForceBreakStart();
    QVERIFY(!deps.screenLockTimer->isActive());
    app.advanceToBreakEnd();

    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void lock_screen_timer_not_running_if_not_idle() {
    auto deps = DummyApp::makeDeps();
    int autoScreenLockSeconds = 20;
    deps.preferences->autoScreenLock->set(autoScreenLockSeconds);

    NiceMock<DummyApp> app(deps);
    app.start();
    app.advance(app.trayData.secondsToNextBreak);

    EXPECT_CALL(app, doLockScreen()).Times(0);
    // Emitted by window manager when user is idle or force break
    app.advanceToForceBreakStart();
    QVERIFY(deps.screenLockTimer->isActive());
    // User is idle during break
    deps.idleTimer->setIdle(true);
    app.advanceToBreakEnd();
    app.advance(1);
    // But is not idle immediately after break
    deps.idleTimer->setIdle(false);
    QVERIFY(!deps.screenLockTimer->isActive());
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
  // During the pause, the count down should not change, and state should be reflected
  void pause_break_on_idle() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    deps.idleTimer->setIdle(true);
    QCOMPARE(app.trayData.pauseReasons, PauseReason::Idle);
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    deps.idleTimer->setIdle(false);
    QVERIFY(!app.trayData.pauseReasons);
    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  // Similar to `pause_break_on_idle`, but for other reasons
  void pause_break_on_request_data() {
    QTest::addColumn<PauseReason>("reason");
    QTest::newRow("battery") << PauseReason::OnBattery;
    QTest::newRow("program") << PauseReason::AppOpen;
  };
  void pause_break_on_request() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    QFETCH(PauseReason, reason);
    emit deps.systemMonitor->pauseRequested(reason);
    QCOMPARE(app.trayData.pauseReasons, reason);
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    emit deps.systemMonitor->resumeRequested(reason);
    QVERIFY(!app.trayData.pauseReasons);
    // Countdown resumed
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
  }
  // If paused for a long time, we consider the user has taken a break and reset timer.
  void reset_countdown_after_pause() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(app.trayData.secondsToNextBreak);
    app.advanceToBreakEnd();
    app.advance(100);

    int smallBreaksBeforeBigBreak = app.trayData.smallBreaksBeforeBigBreak;
    deps.idleTimer->setIdle(true);
    app.advance(deps.preferences->resetAfterPause->get() + 1);
    deps.idleTimer->setIdle(false);

    int smallEvery = deps.preferences->smallEvery->get();
    // Countdown resumed and the time has been reset
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, smallBreaksBeforeBigBreak);

    // Subsequent short idles should not be affected
    deps.idleTimer->setIdle(true);
    app.advance(1);
    deps.idleTimer->setIdle(false);
    // Since the time ellapsed is in paused state, the time should not change
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
  }
  // If paused for even longer, we also reset the break cycle
  void reset_cycle_after_pause() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(100);
    deps.idleTimer->setIdle(true);
    app.advance(deps.preferences->resetCycleAfterPause->get() + 1);
    deps.idleTimer->setIdle(false);

    int smallEvery = deps.preferences->smallEvery->get();
    // Countdown resumed and the time has been reset
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery - 1);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  /* If the time when the pause starts is close to the next break, and the pause is
  not
   * long enough to reset the break countdown, the break will start very soon after
   the
   * pause ends. Having a break right after coming back from other things can be
   * annoying. Therefore, we consider the case where there were no pauses. If the
   break
   * happens and finishes before the user comes back, we just consider the break
   has
   * ended and starts the next cycle.
   */
  void avoid_immediate_break_after_pause() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallEvery = deps.preferences->smallEvery->get();
    app.advance(smallEvery - 30);
    deps.idleTimer->setIdle(true);
    // Enough time paused
    app.advance(30 + deps.preferences->smallFor->get() + 10);
    deps.idleTimer->setIdle(false);
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);
  }
  /* Similar to the previous `avoid_immediate_break_after_pause`, but if the pause
  is so
   * short that we should not consider that the user has taken the break, we will
   not
   * reset the break cycle.
   */
  void do_not_avoid_immediate_break_after_pause_too_short() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallEvery = deps.preferences->smallEvery->get();
    app.advance(smallEvery - 30);
    deps.idleTimer->setIdle(true);
    // Not enough time paused
    app.advance(30 + deps.preferences->smallFor->get() - 10);
    deps.idleTimer->setIdle(false);
    QCOMPARE(app.trayData.secondsToNextBreak, 30);
  }
  /* During a big break, if the user is not ready for a big break, he/she can choose to
   * take a small break instead for now, and the next break is still a big break.
   * Technically, this will close the current big break windows and create new small
   * break windows.
   */
  void take_small_break_instead() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    // Verify we successfully triggered a big break
    EXPECT_CALL(*deps.breakWindows, create(BreakType::Big, _)).Times(1);
    app.bigBreakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));

    // Triggering small break instead is deleting previous windows and creating new ones
    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    EXPECT_CALL(*deps.breakWindows, destroy()).Times(1);
    app.smallBreakInstead();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
    // We are still in breaking mode. Verify the break has not magically ended.
    QVERIFY(app.trayData.isBreaking);

    app.advanceToBreakEnd();
    QVERIFY(!app.trayData.isBreaking);
    // The next break is a big break
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  // User should be able to postpone the break while its happening
  void postpone_while_break() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.breakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));

    EXPECT_CALL(*deps.breakWindows, destroy()).Times(1);
    app.postpone(100);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);
    QCOMPARE(app.trayData.secondsToNextBreak, 100);
    QCOMPARE(app.trayData.secondsToNextBigBreak, 100);
  }
  // We should end the break if the pause happens during the break
  void pause_while_break() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.breakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));

    EXPECT_CALL(*deps.breakWindows, destroy()).Times(1);
    emit deps.systemMonitor->pauseRequested(PauseReason::OnBattery);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
  }
  // Ignore pauses caused by idles during the break
  void idle_while_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.breakNow();
    EXPECT_CALL(*deps.breakWindows, destroy()).Times(0);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
    QVERIFY(app.trayData.isBreaking);
    QCOMPARE(app.trayData.secondsToNextBreak, 0);
  }
  // If the system went asleep during the break, after awaken, the break windows should
  // be closed.
  void sleep_end_while_break() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.breakNow();
    EXPECT_CALL(*deps.breakWindows, destroy()).Times(1);
    emit deps.systemMonitor->sleepEnded(1);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
    QVERIFY(!app.trayData.isBreaking);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  void short_sleep_does_nothing() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(1);
    emit deps.systemMonitor->sleepEnded(1);

    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get() - 1);
  }
  void long_sleep_resets_cycle() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(1);
    emit deps.systemMonitor->sleepEnded(deps.preferences->resetCycleAfterPause->get() +
                                        1);

    // Long sleep should reset both countdown and cycle
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  // Sleep end should not change the pause state
  void sleep_end_while_idle_paused() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    deps.idleTimer->setIdle(true);
    emit deps.systemMonitor->sleepEnded(30);

    // Should remain paused
    QCOMPARE(app.trayData.pauseReasons, PauseReason::Idle);
  }
  // Multiple pause reasons should work
  void more_than_one_pause_reasons() {
    auto deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    emit deps.systemMonitor->pauseRequested(PauseReason::OnBattery);
    QVERIFY(app.trayData.pauseReasons.testFlags(PauseReason::OnBattery));

    deps.idleTimer->setIdle(true);
    QVERIFY(app.trayData.pauseReasons.testFlags(PauseReason::Idle |
                                                PauseReason::OnBattery));
    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);

    deps.idleTimer->setIdle(false);
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
    QVERIFY(app.trayData.pauseReasons.testFlag(PauseReason::OnBattery));

    emit deps.systemMonitor->resumeRequested(PauseReason::OnBattery);
    QVERIFY(!app.trayData.pauseReasons);

    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
    QVERIFY(!app.trayData.pauseReasons);
  }
  /* If the user has postponed the breaks for at least one time, when the user want to
   * postpone again, we are showing a confirmation window telling the user to think
   * twice before postponing the breaks.
   *
   * Note that we should avoid showing the confirmation window if the user clicked
   * postpone right after the previous postpone. In this case, the user is actually
   * postponing once. He/she clicked twice just because the preset durations are not
   * suitable for the current postpone. Therefore, we are determining whether to show
   * the window based one the time since last break. If it is longer than the break
   * schdule, and the user is postponing again (obviously there was at least one
   * postpone), we show this confirm window.
   */
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
  /* In addition to `confirm_postpone_if_long_time_since_last_break`, if we have reset
   * the countdown after the last break, also time since last break is longer than the
   * break schedule, we should not show the confirmation window. In other words, we
   * should also reset time since last break when resetting the break countdown.
   */
  void no_confirm_postpone_if_countdown_reset() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallEvery = deps.preferences->smallEvery->get();
    app.advance(smallEvery - 1);

    emit deps.systemMonitor->sleepEnded(deps.preferences->resetAfterPause->get());
    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery);
    app.advance(smallEvery - 1);

    // Postpone should not trigger confirmation
    EXPECT_CALL(app, confirmPostpone).Times(0);
    app.postpone(100);
    QCOMPARE(app.trayData.secondsToNextBreak, 101);
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  /* We reset the countdown timer if the user is idle for sufficiently long time.
   * However, if the user is postponing the breaks and the current countdown time is
   * longer than a break cycle, then resetting the countdown actually makes break
   * happening earlier instead of later. Therefore, we should avoid resetting the
   * countdown in this case.
   */
  void pause_should_not_affect_postpone() {
    auto deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallEvery = deps.preferences->smallEvery->get();
    app.postpone(1000);

    deps.idleTimer->setIdle(true);
    app.advance(deps.preferences->resetAfterPause->get() + 1);
    deps.idleTimer->setIdle(false);
    app.advance(1);

    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery + 999);
  }
};

QTEST_APPLESS_MAIN(TestApp)
#include "test-app.moc"
