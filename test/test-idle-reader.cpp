// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qtestcase.h>

#include <QObject>
#include <QTest>

#include "core/idle-time.h"
#include "idle/idle-reader.h"

// Pure-logic tests for the idle reader. No Qt platform is needed: the raw reader and
// the inhibited flag are lambdas, so this exercises only the baseline/reset behavior
// that the per-platform monitors feed.
class TestIdleReader : public QObject {
  Q_OBJECT

 private slots:
  void inhibitorOffPassesRawThrough() {
    int raw = 5000;
    bool inhibited = false;
    IdleReader reader([&raw]() { return raw; }, [&inhibited]() { return inhibited; });
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 5000);
    raw = 7000;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 7000);
  }

  void inhibitorOnReportsZero() {
    int raw = 5000;
    bool inhibited = true;
    IdleReader reader([&raw]() { return raw; }, [&inhibited]() { return inhibited; });
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 0);
    raw = 60000;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 0);  // stays zero while inhibited
  }

  void baselineResetOnRelease() {
    int raw = 0;
    bool inhibited = false;
    IdleReader reader([&raw]() { return raw; }, [&inhibited]() { return inhibited; });

    // Inhibited while raw climbs 0 -> 60s; reader reports 0 throughout.
    inhibited = true;
    for (int ms = 0; ms <= 60000; ms += 1000) {
      raw = ms;
      QCOMPARE(reader.read(IdleMode::InhibitorAware), 0);
    }

    // Release at raw=60s: reader reports 0 at release (no instant idleStart) and
    // grows from there, matching Wayland v1 compositor semantics.
    inhibited = false;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 0);  // 60s - 60s baseline
    raw = 61000;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 1000);  // counts from release
    raw = 65000;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 5000);
  }

  void inputDuringInhibitionResetsBaseline() {
    int raw = 0;
    bool inhibited = true;
    IdleReader reader([&raw]() { return raw; }, [&inhibited]() { return inhibited; });

    raw = 30000;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 0);  // baseline captured at 30000

    // User provides input during inhibition that the reader still observes: raw drops.
    raw = 5000;
    QCOMPARE(reader.read(IdleMode::InhibitorAware),
             0);  // still inhibited -> 0, baseline advances to 5000

    // On release the reported value follows raw (baseline reset to the lower value).
    inhibited = false;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 0);  // 5000 - 5000
    raw = 10000;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 5000);  // grows from post-input
  }

  void inputOnlyIgnoresInhibitor() {
    int raw = 5000;
    bool inhibited = true;
    IdleReader reader([&raw]() { return raw; }, [&inhibited]() { return inhibited; });
    // In InputOnly mode the inhibitor is ignored entirely: raw passes through.
    QCOMPARE(reader.read(IdleMode::InputOnly), 5000);
    raw = 8000;
    QCOMPARE(reader.read(IdleMode::InputOnly), 8000);
    // And it must not disturb the baseline used by a later InhibitorAware read.
    inhibited = false;
    QCOMPARE(reader.read(IdleMode::InhibitorAware), 8000);  // raw - 0 baseline
  }

  void errorSentinelPropagated() {
    bool inhibited = true;
    IdleReader reader([]() { return -1; }, [&inhibited]() { return inhibited; });
    // Negative raw is propagated even while inhibited (no false "active" zero).
    QCOMPARE(reader.read(IdleMode::InhibitorAware), -1);
  }
};

QTEST_GUILESS_MAIN(TestIdleReader)
#include "test-idle-reader.moc"
