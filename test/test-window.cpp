// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gmock/gmock.h>
#include <qtestcase.h>

#include <QObject>
#include <QTest>
#include <QTimer>

#include "core/flags.h"
#include "core/window-control.h"
#include "dummy.h"

using testing::_;
using testing::Mock;
using testing::NiceMock;

class TestWindow : public QObject {
  Q_OBJECT
 private slots:
  void init() { QTest::failOnWarning(); }
  void break_data() {
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    BreakData data = windowControl.breakData(SaneBreak::BreakType::Small);
    QCOMPARE(data.totalSeconds, deps.preferences->smallFor->get());
    QVERIFY(deps.preferences->smallMessages->get().contains(data.message));
    QCOMPARE(data.theme.highlightBackground,
             deps.preferences->smallHighlightColor->get());

    data = windowControl.breakData(SaneBreak::BreakType::Big);
    QCOMPARE(data.totalSeconds, deps.preferences->bigFor->get());
    QVERIFY(deps.preferences->bigMessages->get().contains(data.message));
    QCOMPARE(data.theme.highlightBackground,
             deps.preferences->bigHighlightColor->get());
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
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    QFETCH(SaneBreak::BreakType, type);
    windowControl.show(type);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    QFETCH(int, duration);
    int flashFor = deps.preferences->flashFor->get();

    // Never countdown without idle
    EXPECT_CALL(*window, setTime(duration)).Times(flashFor);
    windowControl.advance(deps.preferences->flashFor->get() - 1);
    // Set full-screen after time runs out
    EXPECT_CALL(*window, setFullScreen()).Times(1);
    windowControl.advance(1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(duration - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(duration - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
  }
  // Similar to `tick_with_force_break`, but this time we trigger the idle event so that
  // the window grows to full-screen and count down to zero.
  void tick_without_force_break() {
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
  }
  /* Since we are reusing the `windowControl` object for all the breaks, it's important
   * to make sure that the states are correctly reset between different breaks. Here, we
   * test the case where the user is idled in the first break (and thus force break does
   * not happen) and is not idle in the second break, and therefore we should neither
   * maximize the window nor count down before force break.
   */
  void idle_timer_is_reset() {
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    int smallFor = deps.preferences->smallFor->get();
    int flashFor = deps.preferences->flashFor->get();

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    // During first break, the user is idled
    deps.idleTimer->setIdle(true);
    EXPECT_CALL(*window, setTime(_)).Times(smallFor);
    windowControl.advance(smallFor);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    QVERIFY(!windowControl.hasWindows());

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    window = windowControl.window;

    QVERIFY(!deps.idleTimer->isIdle());

    // During next break, the user is not idled
    // We should not consider the user is idled
    EXPECT_CALL(*window, setTime(smallFor)).Times(flashFor);
    windowControl.advance(flashFor);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    EXPECT_CALL(*window, setTime(_)).Times(smallFor);
    windowControl.advance(smallFor);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    QVERIFY(!windowControl.hasWindows());
  }
  // Similar to `tick_without_force_break`, but there's user activity after maximizing
  void activity_in_break() {
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    // User idled and window is full-screen
    EXPECT_CALL(*window, setFullScreen()).Times(1);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // User activity and window is shrinked
    windowControl.advance(3);
    EXPECT_CALL(*window, resizeToNormal()).Times(1);
    deps.idleTimer->setIdle(false);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // When the prompt is flashing, the count down is paused
    EXPECT_CALL(*window, setTime(smallFor)).Times(3);
    windowControl.advance(3);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // The user is idled and count down all the way to 1
    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Timeout, windows are closed.
    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
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
    auto deps = SimpleWindowControl::makeDeps();
    deps.preferences->confirmAfter->set(10);
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    DummyBreakWindow *window = windowControl.window;

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    deps.idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Should not resize to normal after confirm break
    windowControl.advance(10);
    EXPECT_CALL(*window, resizeToNormal()).Times(0);
    deps.idleTimer->setIdle(false);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    // Qt will delete them later be we need to clear mocks now
    windowControl.deleteWindows();
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
    auto deps = SimpleWindowControl::makeDeps();
    deps.preferences->confirmAfter->set(10);
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    deps.idleTimer->setIdle(true);
    // Fast forward to confirm break
    windowControl.advance(10);
    deps.idleTimer->setIdle(false);

    // Force break is exited
    EXPECT_CALL(*window, resizeToNormal()).Times(1);
    windowControl.exitForceBreak();
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Timer is reset and is not counting down
    EXPECT_CALL(*window, setTime(smallFor)).Times(3);
    windowControl.advance(3);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Qt will delete them later be we need to clear mocks now
    windowControl.deleteWindows();
  }
};

QTEST_MAIN(TestWindow)
#include "test-window.moc"
