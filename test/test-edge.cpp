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

using testing::Mock;
using testing::NiceMock;

class TestEdge : public QObject {
  Q_OBJECT
 private slots:
  void init() { QTest::failOnWarning(); }
  void pauseWhileBreak() {
    auto [deps, windowControl] = DummyApp::makeDeps();
    deps.preferences->pauseOnBattery->set(true);
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*windowControl, createWindows(SaneBreak::BreakType::Small)).Times(1);
    app.breakNow();
    QVERIFY(Mock::VerifyAndClearExpectations(&app));

    EXPECT_CALL(*windowControl, deleteWindows()).Times(1);
    emit deps.systemMonitor->batteryPowered();
    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void idleWhileBreak() {
    auto [deps, windowControl] = DummyApp::makeDeps();
    NiceMock<DummyApp> app(deps);
    app.start();

    EXPECT_CALL(*windowControl, createWindows(SaneBreak::BreakType::Small)).Times(1);
    app.breakNow();
    EXPECT_CALL(*windowControl, deleteWindows()).Times(0);
    emit deps.systemMonitor->idleStarted();

    QVERIFY(Mock::VerifyAndClearExpectations(&app));
  }
  void moreThanOnePauseReasons() {
    auto [deps, windowControl] = DummyApp::makeDeps();
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
};

QTEST_APPLESS_MAIN(TestEdge)
#include "test-edge.moc"
