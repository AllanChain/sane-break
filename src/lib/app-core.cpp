// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app-core.h"

#include <QObject>

#include "lib/flags.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"

AbstractApp::AbstractApp(const AppDependencies &deps, QObject *parent)
    : QObject(parent),
      preferences(deps.preferences),
      m_countDownTimer(deps.countDownTimer),
      m_oneshotIdleTimer(deps.oneshotIdleTimer),
      m_screenLockTimer(deps.screenLockTimer) {
  m_secondsToNextBreak = preferences->smallEvery->get();

  m_countDownTimer->setInterval(1000);
  m_oneshotIdleTimer->setWatchAccuracy(1000);
  m_oneshotIdleTimer->setMinIdleTime(1000);
  m_screenLockTimer->setSingleShot(true);

  // One-shot idle detection right after break end is achieved by making
  // the idle criteria as short as 1 sec, and start the idle timer right
  // after the break starts. We will know if the user is idle after breaks
  // by checking the idle status right after the break.
  // This timer will be deactivated at the first activity after the break.
  connect(m_oneshotIdleTimer, &SystemIdleTime::idleEnd, this,
          &AbstractApp::onOneshotIdleEnd);
  connect(m_countDownTimer, &ITimer::timeout, this, &AbstractApp::tick);
  connect(m_screenLockTimer, &ITimer::timeout, this, &AbstractApp::mayLockScreen);
};

void AbstractApp::start() {
  updateTray();
  m_countDownTimer->start();
}

void AbstractApp::tick() {
  if (m_isBreaking) return;
  if (m_pauseReasons) {
    m_secondsPaused += 1;
    return;
  }
  m_secondsToNextBreak--;
  if (m_secondsToNextBreak <= 0) return breakNow();
  updateTray();
}

void AbstractApp::resetSecondsToNextBreak() {
  m_secondsToNextBreak = preferences->smallEvery->get();
  updateTray();
}

void AbstractApp::breakNow() {
  m_isBreaking = true;
  openBreakWindow(smallBreaksBeforeBig() == 0);
  // Update cycle count after show break
  m_breakCycleCount++;
  updateTray();
  // For testing user is idle after break end
  m_oneshotIdleTimer->startWatching();
}

void AbstractApp::bigBreakNow() {
  m_breakCycleCount = 0;
  breakNow();
}

void AbstractApp::updateTray() {
  int secondsFromLastBreakToNext = preferences->smallEvery->get();
  int secondsToNextBigBreak =
      m_secondsToNextBreak + smallBreaksBeforeBig() * secondsFromLastBreakToNext;
  TrayData data = {
      .isBreaking = m_isBreaking,
      .secondsToNextBreak = m_secondsToNextBreak,
      .secondsToNextBigBreak = secondsToNextBigBreak,
      .secondsFromLastBreakToNext = secondsFromLastBreakToNext,
      .smallBreaksBeforeBigBreak = smallBreaksBeforeBig(),
      .pauseReasons = m_pauseReasons,
  };
  emit trayDataUpdated(data);
}

void AbstractApp::onBreakResume() {
  int msec = preferences->autoScreenLock->get() * 1000;
  if (msec != 0) m_screenLockTimer->start(msec);
}

// Resume countdown if user is idle after breaks
void AbstractApp::onBreakEnd() {
  m_isBreaking = false;
  resetSecondsToNextBreak();
  if (!m_oneshotIdleTimer->isIdle) {
    m_oneshotIdleTimer->stopWatching();
    m_screenLockTimer->stop();
  } else {
    pauseBreak(SaneBreak::PauseReason::Idle);
  }
}

void AbstractApp::pauseBreak(SaneBreak::PauseReasons reason) {
  // Flag should be set before closing windows
  m_pauseReasons |= reason;
  // Stop current break if necessary
  if (reason != SaneBreak::PauseReason::Idle) {
    closeBreakWindow();
  }
  updateTray();
}

void AbstractApp::enableBreak() {
  // Set all flags
  resumeBreak(SaneBreak::PauseReasons::fromInt((1 << 8) - 1));
}

void AbstractApp::resumeBreak(SaneBreak::PauseReasons reason) {
  // Do nothing if not paused
  if (!m_pauseReasons) return;
  // Remove specific reason for pausing
  m_pauseReasons &= ~reason;
  // If there are other reasons for pausing, do nothing
  if (m_pauseReasons) return;

  if (m_secondsPaused > preferences->resetAfterPause->get()) {
    resetSecondsToNextBreak();
  }
  if (m_secondsPaused > preferences->resetCycleAfterPause->get()) {
    m_breakCycleCount = 1;
  }
  updateTray();
}

void AbstractApp::postpone(int secs) {
  m_secondsToNextBreak += secs;
  m_breakCycleCount = 0;  // break after postpone is a big break
  updateTray();
  closeBreakWindow();
}

int AbstractApp::smallBreaksBeforeBig() {
  int breakEvery = preferences->bigAfter->get();
  m_breakCycleCount %= breakEvery;
  return (breakEvery - m_breakCycleCount) % breakEvery;
}

void AbstractApp::onIdleStart() { pauseBreak(SaneBreak::PauseReason::Idle); }

void AbstractApp::onIdleEnd() { resumeBreak(SaneBreak::PauseReason::Idle); }

void AbstractApp::onBattery() {
  if (preferences->pauseOnBattery->get()) pauseBreak(SaneBreak::PauseReason::OnBattery);
}

void AbstractApp::onPower() {
  // No need to check settings because it does nothing if not paused with this
  resumeBreak(SaneBreak::PauseReason::OnBattery);
}

void AbstractApp::onSleepEnd() {
  // We reset these regardless of paused or not
  m_breakCycleCount = 1;
  closeBreakWindow();
  resetSecondsToNextBreak();
}

void AbstractApp::onOneshotIdleEnd() {
  if (m_isBreaking) return;
  m_oneshotIdleTimer->stopWatching();
  resumeBreak(SaneBreak::PauseReason::Idle);
}
