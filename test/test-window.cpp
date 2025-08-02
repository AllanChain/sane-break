// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QTest>
#include <QTimer>

#include "core/break-windows.h"
#include "core/flags.h"
#include "dummy.h"

using testing::_;
using testing::Mock;
using testing::NiceMock;

class TestWindow : public QObject {
  Q_OBJECT
 private slots:
  void init() { QTest::failOnWarning(); }
  void break_data() {
    auto preferences = tempPreferences();
    NiceMock<SimpleBreakWindows> breakWindows;

    breakWindows.create(SaneBreak::BreakType::Small, preferences);
    BreakWindowData data = breakWindows.data;
    QCOMPARE(data.totalSeconds, preferences->smallFor->get());
    QVERIFY(preferences->smallMessages->get().contains(data.message));
    QCOMPARE(data.theme.highlightBackground, preferences->smallHighlightColor->get());

    breakWindows.create(SaneBreak::BreakType::Small, preferences);
    data = breakWindows.data;
    QCOMPARE(data.totalSeconds, preferences->bigFor->get());
    QVERIFY(preferences->bigMessages->get().contains(data.message));
    QCOMPARE(data.theme.highlightBackground, preferences->bigHighlightColor->get());
  }
  // Data for `tick_with_force_break`
  void tick_with_force_break_data() {
    auto preferences = tempPreferences();
    QTest::addColumn<SaneBreak::BreakType>("type");
    QTest::addColumn<int>("duration");

    QTest::newRow("small") << SaneBreak::BreakType::Small
                           << preferences->smallFor->get();
    QTest::newRow("big") << SaneBreak::BreakType::Big << preferences->bigFor->get();
  }
  /* When the timer ticks all the way down without an idle event triggered, the window
   * should first stay in the small prompt mode without counting down until the force
   * break happens. During the force break, the window is maximized and the count down
   * goes to zero. After that, the windows are deleted.
   *
   * We test this for both small and big breaks.
   */
  void tick_with_force_break() {
    auto deps = DummyApp::makeSimpleDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    QFETCH(SaneBreak::BreakType, type);
    if (type == SaneBreak::BreakType::Small) {
      app.breakNow();
    } else {
      app.bigBreakNow();
    }
    QVERIFY(deps.breakWindows->hasWindows());
    DummyBreakWindow *window = deps.breakWindows->window;

    QFETCH(int, duration);
    int flashFor = deps.preferences->flashFor->get();

    // Never countdown without idle
    EXPECT_CALL(*window, setTime(duration)).Times(flashFor);
    app.advance(deps.preferences->flashFor->get() - 1);
    // Set full-screen after time runs out
    EXPECT_CALL(*window, showFullScreen()).Times(1);
    app.advance(1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(duration - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    app.advance(duration - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    app.advance(1);
  }
  // Similar to `tick_with_force_break`, but this time we trigger the idle event so that
  // the window grows to full-screen and count down to zero.
  void tick_without_force_break() {
    auto deps = DummyApp::makeSimpleDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.breakNow();
    QVERIFY(deps.breakWindows->hasWindows());
    DummyBreakWindow *window = deps.breakWindows->window;

    int smallFor = deps.preferences->smallFor->get();

    EXPECT_CALL(*window, showFullScreen()).Times(1);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    app.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    app.advance(1);
    QVERIFY(!deps.breakWindows->hasWindows());
  }
  /* Since we are reusing the `breakWindows` object for all the breaks, it's important
   * to make sure that the states are correctly reset between different breaks. Here, we
   * test if the second break is successfully shown.
   */
  void idle_timer_is_reset() {
    auto deps = DummyApp::makeSimpleDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    int smallFor = deps.preferences->smallFor->get();
    int flashFor = deps.preferences->flashFor->get();

    app.breakNow();
    QVERIFY(deps.breakWindows->hasWindows());
    DummyBreakWindow *window = deps.breakWindows->window;

    // During first break, the user is idled
    deps.idleTimer->setIdle(true);
    EXPECT_CALL(*window, setTime(_)).Times(smallFor);
    app.advance(smallFor);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    QVERIFY(!deps.breakWindows->hasWindows());
    QVERIFY(!app.trayData.isBreaking);

    // User activity after break so that we go to normal state
    deps.idleTimer->setIdle(false);
    app.breakNow();
    QVERIFY(deps.breakWindows->hasWindows());
    window = deps.breakWindows->window;

    // During next break, the user is not idled
    // We should not consider the user is idled
    EXPECT_CALL(*window, setTime(smallFor)).Times(flashFor);
    app.advance(flashFor);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    EXPECT_CALL(*window, setTime(_)).Times(smallFor);
    app.advance(smallFor);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    QVERIFY(!deps.breakWindows->hasWindows());
  }
  // Similar to `tick_without_force_break`, but there's user activity after maximizing
  void activity_in_break() {
    auto deps = DummyApp::makeSimpleDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    app.breakNow();
    QVERIFY(deps.breakWindows->hasWindows());
    DummyBreakWindow *window = deps.breakWindows->window;

    int smallFor = deps.preferences->smallFor->get();

    // User idled and window is full-screen
    EXPECT_CALL(*window, showFullScreen()).Times(1);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // User activity and window is shrinked
    app.advance(3);
    EXPECT_CALL(*window, showFlashPrompt()).Times(1);
    deps.idleTimer->setIdle(false);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // When the prompt is flashing, the count down is paused
    EXPECT_CALL(*window, setTime(smallFor)).Times(3);
    app.advance(3);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // The user is idled and count down all the way to 1
    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    app.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Timeout, windows are closed.
    app.advance(1);
    QVERIFY(!deps.breakWindows->hasWindows());
  }
  /* If user activity and restart the break at anytime, it can be annoying since if the
   * user accidentally moves the mouse near the end of the break, he/she will need to
   * take the break again. Another point is that being able to exit full-screen and
   * continue working is somewhat telling the users that the break is not mandantory and
   * they are still in the state where they can continue work at anytime instead of in a
   * break.
   *
   * However, in most of the time, if the user does not dismiss the full-screen by
   * moving the mouse, they are decided to take the break. Therefore, we are
   * "confirming" the breaks after a certain amount of idle time by ignoring all
   * subsequent user activities and lock in the full-screen mode, similar to the force
   * breaks.
   */
  void confirm_break() {
    auto deps = DummyApp::makeSimpleDeps();
    deps.preferences->confirmAfter->set(10);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.breakNow();
    DummyBreakWindow *window = deps.breakWindows->window;

    EXPECT_CALL(*window, showFullScreen()).Times(1);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Should not resize to normal after confirm break
    app.advance(10);
    EXPECT_CALL(*window, showFlashPrompt()).Times(0);
    deps.idleTimer->setIdle(false);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    // Qt will delete them later be we need to clear mocks now
    deps.breakWindows->destroy();
  }
  /* It's possible that the user have urgent things to do during the force break. Or the
   * user is busy with the current works, but still want to take this break instead of
   * postponing. In both scenarios, the ability to exit force break is useful.
   *
   * Here, we are not doing things like skipping the break or postponing the break.
   * Instead, we reset the window and restarts the break. By restart, we mean also reset
   * the time to the force break. In other words, if the user configured Sane Break to
   * force break after flashing the prompt for 30 sec, then by clicking "Exit Force
   * Break", they will have 30 sec of time before next force break. The user can finish
   * the current work, or choose to postpone the break by interacting with the tray
   * menu.
   */
  void exit_force_break() {
    auto deps = DummyApp::makeSimpleDeps();
    deps.preferences->confirmAfter->set(10);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.breakNow();

    int smallFor = deps.preferences->smallFor->get();

    deps.idleTimer->setIdle(true);
    // Fast forward to confirm break
    app.advance(10);
    deps.idleTimer->setIdle(false);

    // Force break is exited
    EXPECT_CALL(*deps.breakWindows, showFlashPrompt()).Times(1);
    emit deps.breakWindows->exitForceBreakRequested();
    QVERIFY(Mock::VerifyAndClearExpectations(deps.breakWindows));

    DummyBreakWindow *window = deps.breakWindows->window;
    // Timer is reset and is not counting down
    EXPECT_CALL(*window, setTime(smallFor)).Times(3);
    app.advance(3);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Qt will delete them later be we need to clear mocks now
    deps.breakWindows->destroy();
  }
};

QTEST_MAIN(TestWindow)
#include "test-window.moc"
