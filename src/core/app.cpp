// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app.h"

#include <QGuiApplication>
#include <QJsonArray>
#include <QObject>
#include <memory>

#include "break-windows.h"
#include "core/app-data.h"
#include "core/app-states.h"
#include "core/db.h"
#include "core/flags.h"
#include "core/idle-time.h"
#include "core/meeting-prompt.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "core/timer.h"

AbstractApp::AbstractApp(const AppDependencies& deps, QObject* parent)
    : AppContext(parent) {
  data = new AppData(this, deps.preferences);
  preferences = deps.preferences;
  db = deps.db;

  idleTimer = deps.idleTimer;
  if (!idleTimer->parent()) idleTimer->setParent(this);

  screenLockTimer = deps.screenLockTimer;
  screenLockTimer->setSingleShot(true);
  if (!screenLockTimer->parent()) screenLockTimer->setParent(this);

  breakWindows = deps.breakWindows;
  if (!breakWindows->parent()) breakWindows->setParent(this);

  meetingPrompt = deps.meetingPrompt;
  if (!meetingPrompt->parent()) meetingPrompt->setParent(this);

  m_countDownTimer = deps.countDownTimer;
  m_countDownTimer->setInterval(1000);
  if (!m_countDownTimer->parent()) m_countDownTimer->setParent(this);

  m_systemMonitor = deps.systemMonitor;
  if (!m_systemMonitor->parent()) m_systemMonitor->setParent(this);

  connect(data, &AppData::changed, this, &AbstractApp::updateTray);
  connect(m_systemMonitor, &AbstractSystemMonitor::sleepEnded, this,
          &AbstractApp::onSleepEnd);
  connect(m_systemMonitor, &AbstractSystemMonitor::pauseRequested, this,
          &AbstractApp::onPauseRequest);
  connect(m_systemMonitor, &AbstractSystemMonitor::resumeRequested, this,
          &AbstractApp::onResumeRequest);
  connect(idleTimer, &SystemIdleTime::idleStart, this, &AbstractApp::onIdleStart);
  connect(idleTimer, &SystemIdleTime::idleEnd, this, &AbstractApp::onIdleEnd);
  connect(m_countDownTimer, &AbstractTimer::timeout, this, &AbstractApp::tick);
  connect(screenLockTimer, &AbstractTimer::timeout, this, &AbstractApp::doLockScreen);

  connect(breakWindows, &AbstractBreakWindows::lockScreenRequested, this,
          &AbstractApp::doLockScreen);
  connect(breakWindows, &AbstractBreakWindows::exitForceBreakRequested, this,
          [this]() { onMenuAction(Action::ExitForceBreak{}); });

  connect(preferences->pauseOnBattery, &SettingWithSignal::changed, this,
          &AbstractApp::onBatterySettingChange);
  connect(preferences->smallEvery, &SettingWithSignal::changed, this, [this]() {
    if (!this->data->isFocusMode()) this->data->resetSecondsToNextBreak();
  });

  connect(meetingPrompt, &AbstractMeetingPrompt::breakNowRequested, this,
          &AbstractApp::endMeetingBreakNow);
  connect(meetingPrompt, &AbstractMeetingPrompt::breakLaterRequested, this,
          &AbstractApp::endMeetingBreakLater);
  connect(meetingPrompt, &AbstractMeetingPrompt::extendRequested, this,
          &AbstractApp::extendMeeting);
};

void AbstractApp::start() {
  db->logEvent("app::start");
  transitionTo(std::make_unique<AppStateNormal>());
  updateTray();
  idleTimer->startWatching();
  m_countDownTimer->start();
  m_systemMonitor->start();
}

void AbstractApp::updateTray() {
  bool bigEnabled = data->effectiveBigBreakEnabled();
  int secondsFromLastBreakToNext = data->effectiveSmallEvery();
  int secondsToNextBigBreak = 0;
  if (bigEnabled) {
    secondsToNextBigBreak =
        data->secondsToNextBreak() +
        data->smallBreaksBeforeBigBreak() * secondsFromLastBreakToNext;
  }
  TrayData trayData = {
      .isBreaking = data->secondsToNextBreak() == 0,
      .secondsToNextBreak = data->secondsToNextBreak(),
      .secondsToNextBigBreak = secondsToNextBigBreak,
      .secondsFromLastBreakToNext = secondsFromLastBreakToNext,
      .smallBreaksBeforeBigBreak = data->smallBreaksBeforeBigBreak(),
      .bigBreakEnabled = bigEnabled,
      .pauseReasons = data->pauseReasons(),
      .isInMeeting = data->isInMeeting(),
      .meetingSecondsRemaining = data->meetingSecondsRemaining(),
      .meetingTotalSeconds = data->meetingTotalSeconds(),
      .isPostponing = data->isPostponing(),
      .isFocusMode = data->isFocusMode(),
      .focusCyclesRemaining = data->focusCyclesRemaining(),
      .focusTotalCycles = data->focusTotalCycles(),
  };
  emit trayDataUpdated(trayData);
}

void AbstractApp::breakNow() { onMenuAction(Action::BreakNow{}); }
void AbstractApp::bigBreakNow() { onMenuAction(Action::BigBreakNow{}); }
void AbstractApp::enableBreak() { onMenuAction(Action::EnableBreaks{}); }
void AbstractApp::smallBreakInstead() { onMenuAction(Action::SmallBreakInstead{}); }

void AbstractApp::startFocus(int totalCycles, const QString& reason) {
  if (data->isFocusMode() || m_currentState->getID() == AppState::Meeting ||
      data->isPostponing())
    return;
  data->setFocusSpanId(
      db->openSpan("focus", {{"totalCycles", totalCycles}, {"reason", reason}}));
  data->startFocusMode(totalCycles);
  data->earlyBreak();
  transitionTo(std::make_unique<AppStateBreak>());
}

void AbstractApp::endFocus() { onMenuAction(Action::EndFocus{}); }

void AbstractApp::startMeeting(int seconds, const QString& reason) {
  if (m_currentState->getID() == AppState::Meeting || data->isPostponing()) return;
  if (data->isFocusMode()) {
    db->closeSpan(data->focusSpanId(), {{"reason", "meeting"}});
    data->endFocusMode();
  }
  data->setMeetingData(seconds, seconds, reason);
  transitionTo(std::make_unique<AppStateMeeting>());
}

void AbstractApp::endMeetingBreakNow() {
  onMenuAction(Action::EndMeetingBreakNow{});
}

void AbstractApp::endMeetingBreakLater(int seconds) {
  onMenuAction(Action::EndMeetingBreakLater{seconds});
}

void AbstractApp::extendMeeting(int seconds) {
  onMenuAction(Action::ExtendMeeting{seconds});
}

void AbstractApp::postpone(int seconds) {
  // This is defensive. This should already be handled by App.
  if (data->isPostponing() || data->isFocusMode()) return;
  db->logEvent("postpone", {{"seconds", seconds}});
  // Exit current break if we are postponing breaks
  transitionTo(std::make_unique<AppStateNormal>());
  data->postpone(seconds);
}

void AbstractApp::onBatterySettingChange() {
  bool doPause = preferences->pauseOnBattery->get();
  if (!doPause)
    onResumeRequest(PauseReason::OnBattery);
  else if (m_systemMonitor->isOnBattery())
    onPauseRequest(PauseReason::OnBattery);
}

void AbstractApp::onExit() {
  exitCurrentState();
  db->logEvent("app::exit");
}
