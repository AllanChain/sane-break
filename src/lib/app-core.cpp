// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app-core.h"

#include <QObject>

#include "lib/idle-time.h"
#include "lib/preferences.h"

AbstractApp::AbstractApp(const AppDependencies &deps, QObject *parent)
    : QObject(parent),
      preferences(deps.preferences),
      m_countDownTimer(deps.countDownTimer),
      m_oneshotIdleTimer(deps.oneshotIdleTimer),
      m_screenLockTimer(deps.screenLockTimer) {
  secondsToNextBreak = preferences->smallEvery->get();

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
  secondsPaused = pauseReasons ? secondsPaused + 1 : 0;
  secondsToNextBreak--;
  if (secondsToNextBreak <= 0) return breakNow();
  updateTray();
}

void AbstractApp::resetSecondsToNextBreak() {
  secondsToNextBreak = preferences->smallEvery->get();
  updateTray();
}

void AbstractApp::breakNow() {
  isBreaking = true;
  m_countDownTimer->stop();
  openBreakWindow(smallBreaksBeforeBig() == 0);
  // Update cycle count after show break
  breakCycleCount++;
  updateTray();
  // For testing user is idle after break end
  m_oneshotIdleTimer->startWatching();
}

void AbstractApp::onBreakResume() {
  int msec = preferences->autoScreenLock->get() * 1000;
  if (msec == 0 && m_screenLockTimer->isActive())
    m_screenLockTimer->stop();
  else
    m_screenLockTimer->start(msec);
}

// Resume countdown if user is idle after breaks
void AbstractApp::onBreakEnd() {
  isBreaking = false;
  resetSecondsToNextBreak();
  if (!m_oneshotIdleTimer->isIdle) {
    m_oneshotIdleTimer->stopWatching();
    m_screenLockTimer->stop();
    // Continue countdown as normal
    m_countDownTimer->start();
  } else {
    pauseBreak(PauseReason::IDLE);
  }
}

void AbstractApp::pauseBreak(PauseReasons reason) {
  // Flag should be set before closing windows
  pauseReasons |= reason;
  m_countDownTimer->stop();
  // Stop current break if necessary
  if (reason != PauseReason::IDLE) {
    closeBreakWindow();
  }
  updateTray();
}

void AbstractApp::resumeBreak(PauseReasons reason) {
  // Do nothing if not paused
  if (!pauseReasons) return;
  // Remove specific reason for pausing
  pauseReasons &= ~reason;
  // If there are other reasons for pausing, do nothing
  if (pauseReasons) return;

  if (secondsPaused > preferences->resetAfterPause->get()) {
    resetSecondsToNextBreak();
  }
  if (secondsPaused > preferences->resetCycleAfterPause->get()) {
    breakCycleCount = 1;
  }
  m_countDownTimer->start();
  updateTray();
}

void AbstractApp::postpone(int secs) {
  secondsToNextBreak += secs;
  breakCycleCount = 0;  // break after postpone is a big break
  updateTray();
  closeBreakWindow();
}

int AbstractApp::smallBreaksBeforeBig() {
  int breakEvery = preferences->bigAfter->get();
  breakCycleCount %= breakEvery;
  return (breakEvery - breakCycleCount) % breakEvery;
}

void AbstractApp::onIdleStart() { pauseBreak(PauseReason::IDLE); }

void AbstractApp::onIdleEnd() { resumeBreak(PauseReason::IDLE); }

void AbstractApp::onBattery() {
  if (preferences->pauseOnBattery->get()) pauseBreak(PauseReason::ON_BATTERY);
}

void AbstractApp::onPower() {
  // No need to check settings because it does nothing if not paused with this
  resumeBreak(PauseReason::ON_BATTERY);
}

void AbstractApp::onSleepEnd() {
  // We reset these regardless of paused or not
  breakCycleCount = 1;
  closeBreakWindow();
  resetSecondsToNextBreak();
}

void AbstractApp::onOneshotIdleEnd() {
  if (isBreaking) return;
  m_oneshotIdleTimer->stopWatching();
  resumeBreak(PauseReason::IDLE);
}
