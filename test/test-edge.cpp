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

class TestEdge : public QObject {
  Q_OBJECT
 private slots:
  void init() { QTest::failOnWarning(); }
  void pauseWhileBreak() {
    AppDependencies deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(app, openBreakWindow(false)).Times(1);
    app.breakNow();
    EXPECT_CALL(app, closeBreakWindow()).Times(1);
    app.onBattery();

    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void idleWhileBreak() {
    AppDependencies deps = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(app, openBreakWindow(false)).Times(1);
    app.breakNow();
    EXPECT_CALL(app, closeBreakWindow()).Times(0);
    app.onIdleStart();

    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void moreThanOnePauseReasons() {
    AppDependencies deps = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    app.onBattery();
    app.onIdleStart();

    // Countdown stopped
    int secondsToNextBreak = app.trayData.secondsToNextBreak;
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
    QVERIFY(app.trayData.pauseReasons.testFlags(SaneBreak::PauseReason::Idle |
                                                SaneBreak::PauseReason::OnBattery));

    app.onIdleEnd();
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak);
    QVERIFY(app.trayData.pauseReasons.testFlag(SaneBreak::PauseReason::OnBattery));

    app.onPower();
    app.advance(1);
    QCOMPARE(app.trayData.secondsToNextBreak, secondsToNextBreak - 1);
    QVERIFY(!app.trayData.pauseReasons);
  }
};

QTEST_APPLESS_MAIN(TestEdge)
#include "test-edge.moc"
