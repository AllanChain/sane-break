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
  void tick_with_force_break_data() {
    auto preferences = tempPreferences();
    QTest::addColumn<SaneBreak::BreakType>("type");
    QTest::addColumn<int>("duration");

    QTest::newRow("small") << SaneBreak::BreakType::Small
                           << preferences->smallFor->get();
    QTest::newRow("big") << SaneBreak::BreakType::Big << preferences->bigFor->get();
  }
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
  void tick_without_force_break() {
    auto idleTimer = new DummyIdleTime();
    WindowDependencies deps = {tempPreferences(), idleTimer};
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
  }
  void activity_in_break() {
    auto idleTimer = new DummyIdleTime();
    WindowDependencies deps = {tempPreferences(), idleTimer};
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(3);
    EXPECT_CALL(*window, resizeToNormal()).Times(1);
    idleTimer->setIdle(false);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(smallFor)).Times(3);
    windowControl.advance(3);
    idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
  }
  void confirm_break() {
    auto preferences = tempPreferences();
    preferences->confirmAfter->set(10);
    auto idleTimer = new DummyIdleTime();
    WindowDependencies deps = {preferences, idleTimer};
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    DummyBreakWindow *window = windowControl.window;

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    idleTimer->setIdle(true);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Should not resize to normal after confirm break
    windowControl.advance(10);
    EXPECT_CALL(*window, resizeToNormal()).Times(0);
    idleTimer->setIdle(false);
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    // Qt will delete them later be we need to clear mocks now
    windowControl.deleteWindows();
  }
  void exit_force_break() {
    auto preferences = tempPreferences();
    preferences->confirmAfter->set(10);
    auto idleTimer = new DummyIdleTime();
    WindowDependencies deps = {preferences, idleTimer};
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    idleTimer->setIdle(true);
    // Fast forward to confirm break
    windowControl.advance(10);
    idleTimer->setIdle(false);

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
