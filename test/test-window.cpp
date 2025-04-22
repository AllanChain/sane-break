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
  void breakData() {
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
  void tickWithForceBreak() {
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();
    int flashFor = deps.preferences->flashFor->get();

    // Never countdown without idle
    EXPECT_CALL(*window, setTime(smallFor)).Times(flashFor);
    windowControl.advance(deps.preferences->flashFor->get() - 1);
    // Set full-screen after time runs out
    EXPECT_CALL(*window, setFullScreen()).Times(1);
    windowControl.advance(1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
  }
  void tickWithoutForceBreak() {
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    emit deps.idleTimer->idleStart();
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
  }
  void activityInBreak() {
    auto deps = SimpleWindowControl::makeDeps();
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    QVERIFY(windowControl.hasWindows());
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    emit deps.idleTimer->idleStart();
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(3);
    EXPECT_CALL(*window, resizeToNormal()).Times(1);
    emit deps.idleTimer->idleEnd();
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(smallFor)).Times(3);
    windowControl.advance(3);
    emit deps.idleTimer->idleStart();
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    EXPECT_CALL(*window, setTime(_)).Times(smallFor - 2);
    EXPECT_CALL(*window, setTime(1));
    EXPECT_CALL(*window, setTime(0)).Times(0);
    windowControl.advance(smallFor - 1);
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    windowControl.advance(1);
    QVERIFY(!windowControl.hasWindows());
  }
  void confirmBreak() {
    auto deps = SimpleWindowControl::makeDeps();
    deps.preferences->confirmAfter->set(10);
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    DummyBreakWindow *window = windowControl.window;

    EXPECT_CALL(*window, setFullScreen()).Times(1);
    emit deps.idleTimer->idleStart();
    QVERIFY(Mock::VerifyAndClearExpectations(window));

    // Should not resize to normal after confirm break
    windowControl.advance(10);
    EXPECT_CALL(*window, resizeToNormal()).Times(0);
    emit deps.idleTimer->idleEnd();
    QVERIFY(Mock::VerifyAndClearExpectations(window));
    // Qt will delete them later be we need to clear mocks now
    windowControl.deleteWindows();
  }
  void exitForceBreak() {
    auto deps = SimpleWindowControl::makeDeps();
    deps.preferences->confirmAfter->set(10);
    NiceMock<SimpleWindowControl> windowControl(deps);

    windowControl.show(SaneBreak::BreakType::Small);
    DummyBreakWindow *window = windowControl.window;

    int smallFor = deps.preferences->smallFor->get();

    emit deps.idleTimer->idleStart();
    // Fast forward to confirm break
    windowControl.advance(10);
    emit deps.idleTimer->idleEnd();

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
