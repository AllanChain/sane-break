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

using testing::Mock, testing::NiceMock, testing::_;

class TestApp : public QObject {
  Q_OBJECT
 private:
  QObject* depsParent;
  DummyAppDependencies deps;
 private slots:
  void init() {
    QTest::failOnWarning();
    depsParent = new QObject();
    deps = DummyApp::makeDeps(depsParent);
  }
  void cleanup() { delete depsParent; }
  void app_initial_state() {
    NiceMock<DummyApp> app(deps);
    app.start();

    QCOMPARE(app.trayData.pauseReasons.toInt(), 0);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  void tick() {
    NiceMock<DummyApp> app(deps);
    app.start();

    QVERIFY(deps.countDownTimer->isActive());

    emit deps.countDownTimer->timeout();
    // Time has ellapsed
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get() - 1);
  }
  // Simple test if the break window shows after the countdown ends.
  void show_first_break() {
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
    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.postpone(100);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak + 100);

    app.advance(app.trayData.secondsToNextBreak);
    app.advanceToForceBreakStart();
    app.advance(deps.preferences->smallFor->get() + 1);
    // Break time is extended
    QVERIFY(app.trayData.isBreaking);

    app.advance(app.trayData.secondsToNextBreak);
    app.advanceToBreakEnd();
    // Next work session is shortened
    QCOMPARE(app.trayData.secondsToNextBreak,
             deps.preferences->smallEvery->get() - 100);
  }
  // We disallow postponing twice in the same work session
  void cannot_postpone_twice() {
    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.postpone(100);
    app.postpone(100);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak + 100);
  }
  // We allow postponing across different work sessions
  void postpone_again_after_break() {
    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.postpone(100);

    app.advance(app.trayData.secondsToNextBreak);
    app.advanceToBreakEnd();
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 100);
    app.postpone(300);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak + 200);
  }
  // During the pause, the count down should not change, and state should be reflected
  void pause_break_on_idle() {
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
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.breakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));

    EXPECT_CALL(*deps.breakWindows, destroy()).Times(1);
    app.postpone(100);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
    QCOMPARE(app.trayData.secondsToNextBreak, 100);
  }
  // We should end the break if the pause happens during the break
  void pause_while_break() {
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
    NiceMock<DummyApp> app(deps);
    app.start();

    app.advance(1);
    emit deps.systemMonitor->sleepEnded(1);

    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get() - 1);
  }
  void long_sleep_resets_cycle() {
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
  // Meeting mode: break countdown frozen during meeting
  void meeting_freezes_break_countdown() {
    NiceMock<DummyApp> app(deps);
    app.start();

    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.startMeeting(3600, "standup");
    QVERIFY(app.trayData.isInMeeting);

    app.advance(10);
    // Break countdown should not change
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
  }
  // Meeting countdown ticks and shows end prompt at 0
  void meeting_countdown_ticks() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    QCOMPARE(app.trayData.meetingSecondsRemaining, 5);

    app.advance(3);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 2);

    EXPECT_CALL(*deps.meetingPrompt, showEndPrompt()).Times(1);
    app.advance(2);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.meetingPrompt));
    QCOMPARE(app.trayData.meetingSecondsRemaining, 0);
    QVERIFY(app.trayData.isInMeeting);  // still active in awaiting state
  }
  // Prompt's internal timeout fires breakLaterRequested(0), ending meeting
  void meeting_prompt_timeout() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    EXPECT_CALL(*deps.meetingPrompt, showEndPrompt()).Times(1);
    app.advance(5);  // countdown ends
    QVERIFY(Mock::VerifyAndClearExpectations(deps.meetingPrompt));

    // Simulate the prompt's internal timeout firing
    emit deps.meetingPrompt->breakLaterRequested(0);
    QVERIFY(!app.trayData.isInMeeting);
    QVERIFY(app.trayData.isBreaking);
  }
  // Idle during meeting stays in Normal
  void meeting_blocks_idle_pause() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(3600, "standup");
    deps.idleTimer->setIdle(true);

    QCOMPARE(app.currentState(), AppState::Meeting);
    QVERIFY(!app.trayData.pauseReasons);
  }
  // System pause during meeting stays in Meeting
  void meeting_blocks_system_pause() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(3600, "standup");
    emit deps.systemMonitor->pauseRequested(PauseReason::OnBattery);

    QCOMPARE(app.currentState(), AppState::Meeting);
  }
  // endMeetingBreakLater sets 5min countdown and big break
  void meeting_end_break_soon() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    app.advance(5);
    app.endMeetingBreakLater(300);

    QVERIFY(!app.trayData.isInMeeting);
    QCOMPARE(app.trayData.secondsToNextBreak, 300);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);  // next is big
  }
  // endMeetingBreakNow triggers immediate big break
  void meeting_end_break_now() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    app.advance(5);
    app.endMeetingBreakLater();

    QVERIFY(!app.trayData.isInMeeting);
    QVERIFY(app.trayData.isBreaking);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);  // big break
  }
  // Extend meeting resets awaiting state
  void meeting_extend() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    EXPECT_CALL(*deps.meetingPrompt, showEndPrompt()).Times(1);
    app.advance(5);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.meetingPrompt));

    app.extendMeeting(1800);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 1800);
    QVERIFY(app.trayData.isInMeeting);
  }
  // Sleep during meeting adjusts countdown
  void sleep_during_meeting_adjusts() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(3600, "standup");
    emit deps.systemMonitor->sleepEnded(600);

    QCOMPARE(app.trayData.meetingSecondsRemaining, 3000);
    QVERIFY(app.trayData.isInMeeting);
  }
  // Sleep past meeting end resets cycle
  void sleep_past_meeting_resets_cycle() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(600, "standup");
    emit deps.systemMonitor->sleepEnded(1200);

    QVERIFY(!app.trayData.isInMeeting);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);
  }
  // Short sleep during awaiting resets prompt timeout
  void sleep_during_awaiting_short() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    app.advance(5);  // enter awaiting

    EXPECT_CALL(*deps.meetingPrompt, resetTimeout()).Times(1);
    emit deps.systemMonitor->sleepEnded(10);  // < bigFor
    QVERIFY(app.trayData.isInMeeting);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.meetingPrompt));
  }
  // Long sleep during awaiting ends meeting
  void sleep_during_awaiting_long() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    app.advance(5);

    emit deps.systemMonitor->sleepEnded(deps.preferences->bigFor->get());
    QVERIFY(!app.trayData.isInMeeting);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  // When big breaks are disabled, all breaks should be small
  void big_break_disabled_all_breaks_small() {
    deps.preferences->bigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    int numberOfBreaks = deps.preferences->bigAfter->get();
    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(numberOfBreaks);
    EXPECT_CALL(*deps.breakWindows, create(BreakType::Big, _)).Times(0);
    for (int j = 0; j < numberOfBreaks; j++) {
      app.advance(app.trayData.secondsToNextBreak);
      app.advanceToBreakEnd();
    }
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
  }
  // When big breaks are disabled, tray should not show big break info
  void big_break_disabled_tray_info() {
    deps.preferences->bigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    QVERIFY(!app.trayData.bigBreakEnabled);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, -1);
  }
  // When big breaks are disabled, bigBreakNow should trigger a small break
  void big_break_disabled_big_break_now() {
    deps.preferences->bigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.bigBreakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
  }
  // When big breaks are disabled, meeting end should not force a big break
  void big_break_disabled_meeting_end() {
    deps.preferences->bigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(5, "standup");
    app.advance(5);
    app.endMeetingBreakLater(0);

    QVERIFY(!app.trayData.isInMeeting);
    QVERIFY(app.trayData.isBreaking);
    // Should NOT be a big break since big breaks are disabled
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, -1);
  }
  // Meeting blocked during postpone
  void meeting_blocked_during_postpone() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.postpone(100);
    app.startMeeting(3600, "standup");
    QVERIFY(!app.trayData.isInMeeting);
  }
  // Meeting during break exits break
  void meeting_during_break_exits_break() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.breakNow();
    QVERIFY(app.trayData.isBreaking);

    app.startMeeting(3600, "standup");
    QVERIFY(app.trayData.isInMeeting);
    QVERIFY(!app.trayData.isBreaking);
  }
  // TrayData reflects meeting state
  void meeting_tray_data() {
    NiceMock<DummyApp> app(deps);
    app.start();

    QVERIFY(!app.trayData.isInMeeting);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 0);

    app.startMeeting(100, "standup");
    QVERIFY(app.trayData.isInMeeting);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 100);

    app.advance(50);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 50);
  }
  // Sleep end should not change the pause state
  void sleep_end_while_idle_paused() {
    NiceMock<DummyApp> app(deps);
    app.start();

    deps.idleTimer->setIdle(true);
    emit deps.systemMonitor->sleepEnded(30);

    // Should remain paused
    QCOMPARE(app.trayData.pauseReasons, PauseReason::Idle);
  }
  // Multiple pause reasons should work
  void more_than_one_pause_reasons() {
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
  /* We disallow pausing duing postponing because:
   * 1. this will almost not happen, so we can simplify things.
   * 2. if this happens, postponing should mean continuous working.
   */
  void no_pause_during_break() {
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallEvery = deps.preferences->smallEvery->get();
    app.postpone(1000);

    deps.idleTimer->setIdle(true);
    app.advance(10);
    deps.idleTimer->setIdle(false);
    app.advance(1);

    QCOMPARE(app.trayData.secondsToNextBreak, smallEvery + 989);
  }
  // End meeting early with immediate break
  void meeting_end_early_break_now() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(3600, "standup");
    app.advance(600);  // 10 minutes in
    QVERIFY(app.trayData.isInMeeting);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 3000);

    app.endMeetingBreakLater(0);
    QVERIFY(!app.trayData.isInMeeting);
    QVERIFY(app.trayData.isBreaking);
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);  // big break
  }
  // Focus mode entry break forces full-screen (flashFor=0)
  void focus_mode_entry_break() {
    NiceMock<DummyApp> app(deps);
    app.start();

    // Set focus schedule
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);

    EXPECT_CALL(*deps.breakWindows, create(BreakType::Small, _)).Times(1);
    app.startFocus(4, "deep work");
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
    QVERIFY(app.trayData.isBreaking);
    QVERIFY(app.trayData.isFocusMode);

    // Entry break should be force break immediately (flashFor=0),
    // so full screen should show without any ticks
    EXPECT_CALL(*deps.breakWindows, showFullScreen()).Times(1);
    app.advance(1);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));
  }
  // Each break decrements cycles and uses effective schedule
  void focus_mode_cycles_decrement() {
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startFocus(3, "deep work");
    QVERIFY(app.trayData.isFocusMode);
    QCOMPARE(app.trayData.focusCyclesRemaining, 3);

    // Finish entry break
    app.advanceToBreakEnd();
    QCOMPARE(app.trayData.focusCyclesRemaining, 2);
    // Uses focus schedule (600s not default 1200s)
    QCOMPARE(app.trayData.secondsToNextBreak, 600);
  }
  // Focus deactivates when cycles reach 0, normal schedule resumes
  void focus_mode_exit_on_last_break() {
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startFocus(2, "deep work");
    // Entry break
    app.advanceToBreakEnd();
    QCOMPARE(app.trayData.focusCyclesRemaining, 1);
    QVERIFY(app.trayData.isFocusMode);

    // Second break
    app.advance(app.trayData.secondsToNextBreak);
    app.advanceToBreakEnd();
    // Focus mode should now be deactivated
    QVERIFY(!app.trayData.isFocusMode);
    // Normal schedule resumes
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  // EndFocus sets cycles=1, triggers break, exits focus
  void focus_mode_early_exit() {
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startFocus(4, "deep work");
    app.advanceToBreakEnd();
    QVERIFY(app.trayData.isFocusMode);
    QCOMPARE(app.trayData.focusCyclesRemaining, 3);

    // End focus early
    app.endFocus();
    QVERIFY(app.trayData.isBreaking);
    // Finish the exit break
    app.advanceToBreakEnd();
    QVERIFY(!app.trayData.isFocusMode);
    QCOMPARE(app.trayData.secondsToNextBreak, deps.preferences->smallEvery->get());
  }
  // Postpone blocked during focus
  void focus_mode_no_postpone() {
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startFocus(3, "deep work");
    app.advanceToBreakEnd();
    int secondsToNextBreak = app.trayData.secondsToNextBreak;

    app.postpone(100);
    // Postpone should be blocked
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
  }
  // Starting meeting clears focus
  void focus_mode_meeting_ends_focus() {
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startFocus(3, "deep work");
    app.advanceToBreakEnd();
    QVERIFY(app.trayData.isFocusMode);

    app.startMeeting(3600, "standup");
    QVERIFY(!app.trayData.isFocusMode);
    QVERIFY(app.trayData.isInMeeting);
  }
  // breakCycleCount increments during focus and carries over after focus ends
  void focus_mode_shared_cycle() {
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    // Start at cycle 1 with bigAfter=3, so 2 small breaks before big
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak,
             deps.preferences->bigAfter->get() - 1);

    app.startFocus(2, "deep work");
    // Entry break (cycle 1→2)
    app.advanceToBreakEnd();
    // Second focus break (cycle 2→3)
    app.advance(app.trayData.secondsToNextBreak);
    app.advanceToBreakEnd();
    // Focus ended, back to normal schedule
    QVERIFY(!app.trayData.isFocusMode);
    // After 2 breaks from cycle 1 with bigAfter=3, next break is big
    QCOMPARE(app.trayData.smallBreaksBeforeBigBreak, 0);
  }
  // Tray data reflects focus state
  void focus_mode_tray_info() {
    deps.preferences->focusSmallEvery->set(600);
    deps.preferences->focusSmallFor->set(10);
    deps.preferences->focusBigBreakEnabled->set(false);
    NiceMock<DummyApp> app(deps);
    app.start();

    QVERIFY(!app.trayData.isFocusMode);
    QCOMPARE(app.trayData.focusCyclesRemaining, 0);
    QCOMPARE(app.trayData.focusTotalCycles, 0);

    app.startFocus(4, "deep work");
    QVERIFY(app.trayData.isFocusMode);
    QCOMPARE(app.trayData.focusCyclesRemaining, 4);
    QCOMPARE(app.trayData.focusTotalCycles, 4);

    app.advanceToBreakEnd();
    QCOMPARE(app.trayData.focusCyclesRemaining, 3);
    QCOMPARE(app.trayData.focusTotalCycles, 4);
  }
  // Extend meeting before it ends
  void meeting_extend_before_end() {
    NiceMock<DummyApp> app(deps);
    app.start();

    app.startMeeting(600, "standup");
    app.advance(100);
    QCOMPARE(app.trayData.meetingSecondsRemaining, 500);

    EXPECT_CALL(*deps.meetingPrompt, showEndPrompt()).Times(0);
    app.extendMeeting(900);  // +15 min
    QCOMPARE(app.trayData.meetingSecondsRemaining, 1400);
    QVERIFY(app.trayData.isInMeeting);
    QVERIFY(Mock::VerifyAndClearExpectations(deps.meetingPrompt));
  }
};

QTEST_MAIN(TestApp)
#include "test-app.moc"
