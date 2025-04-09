// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app.h"

#include <QObject>

#include "core/flags.h"
#include "core/idle-time.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"
#include "core/window-control.h"

AbstractApp::AbstractApp(const AppDependencies &deps, QObject *parent)
    : QObject(parent),
      preferences(deps.preferences),
      m_countDownTimer(deps.countDownTimer),
      m_oneshotIdleTimer(deps.oneshotIdleTimer),
      m_screenLockTimer(deps.screenLockTimer),
      m_systemMonitor(deps.systemMonitor),
      m_windowControl(deps.windowControl) {
  if (!m_countDownTimer->parent()) m_countDownTimer->setParent(this);
  if (!m_oneshotIdleTimer->parent()) m_oneshotIdleTimer->setParent(this);
  if (!m_screenLockTimer->parent()) m_screenLockTimer->setParent(this);
  if (!m_systemMonitor->parent()) m_systemMonitor->setParent(this);
  if (!m_windowControl->parent()) m_windowControl->setParent(this);

  m_secondsToNextBreak = preferences->smallEvery->get();

  m_countDownTimer->setInterval(1000);
  m_oneshotIdleTimer->setWatchAccuracy(1000);
  m_oneshotIdleTimer->setMinIdleTime(1000);
  m_screenLockTimer->setSingleShot(true);

  connect(m_windowControl, &AbstractWindowControl::countDownStateChanged, this,
          &AbstractApp::onBreakCountDownStateChange);
  connect(m_windowControl, &AbstractWindowControl::timeout, this,
          &AbstractApp::onBreakEnd);
  connect(m_systemMonitor, &AbstractSystemMonitor::idleStarted, this,
          &AbstractApp::onIdleStart);
  connect(m_systemMonitor, &AbstractSystemMonitor::idleEnded, this,
          &AbstractApp::onIdleEnd);
  connect(m_systemMonitor, &AbstractSystemMonitor::sleepEnded, this,
          &AbstractApp::onSleepEnd);
  connect(m_systemMonitor, &AbstractSystemMonitor::batteryPowered, this,
          &AbstractApp::onBattery);
  connect(m_systemMonitor, &AbstractSystemMonitor::adaptorPowered, this,
          &AbstractApp::onPower);
  connect(m_systemMonitor, &AbstractSystemMonitor::programStarted, this,
          [this]() { pauseBreak(SaneBreak::PauseReason::AppOpen); });
  connect(m_systemMonitor, &AbstractSystemMonitor::programStopped, this,
          [this]() { resumeBreak(SaneBreak::PauseReason::AppOpen); });
  // One-shot idle detection right after break end is achieved by making
  // the idle criteria as short as 1 sec, and start the idle timer right
  // after the break starts. We will know if the user is idle after breaks
  // by checking the idle status right after the break.
  // This timer will be deactivated at the first activity after the break.
  connect(m_oneshotIdleTimer, &SystemIdleTime::idleEnd, this,
          &AbstractApp::onOneshotIdleEnd);
  connect(m_countDownTimer, &AbstractTimer::timeout, this, &AbstractApp::tick);
  connect(m_screenLockTimer, &AbstractTimer::timeout, this, &AbstractApp::doLockScreen);

  connect(preferences->pauseOnBattery, &SettingWithSignal::changed, this,
          &AbstractApp::onBatterySettingChange);
  connect(preferences->smallEvery, &SettingWithSignal::changed, this,
          &AbstractApp::resetSecondsToNextBreak);
};

void AbstractApp::start() {
  updateTray();
  m_countDownTimer->start();
  m_systemMonitor->start();
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
  m_windowControl->show(smallBreaksBeforeBig() == 0 ? SaneBreak::BreakType::Big
                                                    : SaneBreak::BreakType::Small);
  updateTray();
  // For testing user is idle after break end
  m_oneshotIdleTimer->startWatching();
}

void AbstractApp::bigBreakNow() {
  m_breakCycleCount = 0;
  breakNow();
}

void AbstractApp::smallBreakInstead() {
  m_windowControl->close();
  // Next break will be a big break
  m_breakCycleCount = -1;
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

void AbstractApp::onBreakCountDownStateChange(bool countingDown) {
  if (!countingDown) return m_screenLockTimer->stop();
  int msec = preferences->autoScreenLock->get() * 1000;
  if (msec != 0) m_screenLockTimer->start(msec);
}

void AbstractApp::onBreakEnd() {
  m_isBreaking = false;
  resetSecondsToNextBreak();
  m_breakCycleCount++;
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
    m_windowControl->close();
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
  m_windowControl->close();
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
  m_windowControl->close();
  resetSecondsToNextBreak();
}

void AbstractApp::onOneshotIdleEnd() {
  if (m_isBreaking) return;
  m_oneshotIdleTimer->stopWatching();
  resumeBreak(SaneBreak::PauseReason::Idle);
}

void AbstractApp::onBatterySettingChange() {
  bool doPause = preferences->pauseOnBattery->get();
  if (!doPause)
    resumeBreak(SaneBreak::PauseReason::OnBattery);
  else if (m_systemMonitor->isOnBattery())
    pauseBreak(SaneBreak::PauseReason::OnBattery);
}
