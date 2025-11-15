// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/app.h"

#include <QGuiApplication>
#include <QObject>
#include <memory>

#include "break-windows.h"
#include "core/app-data.h"
#include "core/app-states.h"
#include "core/db.h"
#include "core/flags.h"
#include "core/idle-time.h"
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
          [this]() { onMenuAction(MenuAction::ExitForceBreak); });

  connect(preferences->pauseOnBattery, &SettingWithSignal::changed, this,
          &AbstractApp::onBatterySettingChange);
  connect(preferences->smallEvery, &SettingWithSignal::changed, this,
          [this]() { this->data->resetSecondsToNextBreak(); });
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
  int secondsFromLastBreakToNext = preferences->smallEvery->get();
  int secondsToNextBigBreak =
      data->secondsToNextBreak() +
      data->smallBreaksBeforeBigBreak() * secondsFromLastBreakToNext;
  TrayData trayData = {
      .isBreaking = data->secondsToNextBreak() == 0,
      // .isBreaking = m_currentState->getID() == AppState::Break,
      .secondsToNextBreak = data->secondsToNextBreak(),
      .secondsToNextBigBreak = secondsToNextBigBreak,
      .secondsFromLastBreakToNext = secondsFromLastBreakToNext,
      .smallBreaksBeforeBigBreak = data->smallBreaksBeforeBigBreak(),
      .pauseReasons = data->pauseReasons(),
  };
  emit trayDataUpdated(trayData);
}

void AbstractApp::breakNow() {
  m_currentState->onMenuAction(this, MenuAction::BreakNow);
}
void AbstractApp::bigBreakNow() {
  m_currentState->onMenuAction(this, MenuAction::BigBreakNow);
}
void AbstractApp::enableBreak() {
  m_currentState->onMenuAction(this, MenuAction::EnableBreaks);
}
void AbstractApp::smallBreakInstead() {
  m_currentState->onMenuAction(this, MenuAction::SmallBreakInstead);
}

void AbstractApp::postpone(int secs) {
  bool needsConfirm =
      data->secondsSinceLastBreak() > preferences->smallEvery->get() + 60;
  if (needsConfirm && !confirmPostpone(secs)) return;
  db->logEvent("postpone", {{"seconds", secs}});
  data->addSecondsToNextBreak(secs);
  data->makeNextBreakBig();
  if (m_currentState->getID() == AppState::Break) {
    transitionTo(std::make_unique<AppStateNormal>());
  }
}

void AbstractApp::onSleepEnd(int sleptSeconds) {
  onPauseRequest(PauseReason::Sleep);
  // Timers does not tick during sleep, therefore we need to add them back.
  data->addSecondsPaused(sleptSeconds);
  onResumeRequest(PauseReason::Sleep);
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
  db->logEvent("app:exit");
}
