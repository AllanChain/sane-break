// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <qglobal.h>
#include <qlogging.h>

#include <QApplication>
#include <QDateTime>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStyleHints>
#include <QTimer>
#include <QWindow>
#include <Qt>

#include "gui/pref-window.h"
#include "gui/tray.h"
#include "gui/widgets/language-select.h"
#include "gui/window-manager.h"
#include "lib/app-core.h"
#include "lib/flags.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"
#include "lib/screen-lock.h"
#include "lib/system-monitor.h"

SaneBreakApp::SaneBreakApp(const AppDependencies &deps, QObject *parent)
    : AbstractApp(deps, parent) {
  prefWindow = new PreferenceWindow(preferences);
  breakManager = new BreakWindowManager(preferences, this);
  systemMonitor = new SystemMonitor(preferences, this);
  tray = StatusTrayWindow::createTrayOrWindow(preferences, this);

  connect(breakManager, &BreakWindowManager::resume, this,
          &SaneBreakApp::onBreakResume);
  connect(breakManager, &BreakWindowManager::timeout, this, &SaneBreakApp::onBreakEnd);
  connect(this, &SaneBreakApp::trayDataUpdated, tray, &StatusTrayWindow::update);
  connect(tray, &StatusTrayWindow::nextBreakRequested, this, &SaneBreakApp::breakNow);
  connect(tray, &StatusTrayWindow::nextBigBreakRequested, this,
          &SaneBreakApp::bigBreakNow);
  connect(tray, &StatusTrayWindow::postponeRequested, this, &SaneBreakApp::postpone);
  connect(tray, &StatusTrayWindow::preferenceWindowRequested, this,
          &SaneBreakApp::showPreferences);
  connect(tray, &StatusTrayWindow::enableBreakRequested, this,
          &SaneBreakApp::enableBreak);
  connect(tray, &StatusTrayWindow::quitRequested, this, &SaneBreakApp::confirmQuit);
  connect(systemMonitor, &SystemMonitor::idleStarted, this, &SaneBreakApp::onIdleStart);
  connect(systemMonitor, &SystemMonitor::idleEnded, this, &SaneBreakApp::onIdleEnd);
  connect(systemMonitor, &SystemMonitor::sleepEnded, this, &SaneBreakApp::onSleepEnd);
  connect(systemMonitor, &SystemMonitor::batteryPowered, this,
          &SaneBreakApp::onBattery);
  connect(systemMonitor, &SystemMonitor::adaptorPowered, this, &SaneBreakApp::onPower);
  connect(systemMonitor, &SystemMonitor::programStarted, this,
          [this]() { pauseBreak(SaneBreak::PauseReason::AppOpen); });
  connect(systemMonitor, &SystemMonitor::programStopped, this,
          [this]() { resumeBreak(SaneBreak::PauseReason::AppOpen); });
  connect(preferences->pauseOnBattery, &SettingWithSignal::changed, this,
          &SaneBreakApp::onBatterySettingChange);
  connect(preferences->smallEvery, &SettingWithSignal::changed, this,
          &SaneBreakApp::resetSecondsToNextBreak);
  connect(preferences->language, &SettingWithSignal::changed, this,
          [this]() { LanguageSelect::setLanguage(preferences->language->get()); });
  connect(this, &SaneBreakApp::quit, qApp, &QApplication::quit, Qt::QueuedConnection);
}

SaneBreakApp *SaneBreakApp::create(SanePreferences *preferences, QObject *parent) {
  auto countDownTimer = new Timer();
  auto oneshotIdleTimer = SystemIdleTime::createIdleTimer();
  auto screenLockTimer = new Timer();

  AppDependencies deps = {.countDownTimer = countDownTimer,
                          .oneshotIdleTimer = oneshotIdleTimer,
                          .screenLockTimer = screenLockTimer,
                          .preferences = preferences};
  return new SaneBreakApp(deps, parent);
}

void SaneBreakApp::start() {
  AbstractApp::start();
  tray->show();
  systemMonitor->start();
}

void SaneBreakApp::openBreakWindow(bool isBigBreak) {
  breakManager->show(isBigBreak ? SaneBreak::BreakType::Big
                                : SaneBreak::BreakType::Small);
}

void SaneBreakApp::closeBreakWindow() { breakManager->close(); }

void SaneBreakApp::mayLockScreen() {
  if (preferences->autoScreenLock->get()) {
    if (lockScreen())
      qDebug("Screen locked");
    else
      qWarning("Failed to lock screen");
  }
}

void SaneBreakApp::showPreferences() {
  prefWindow->show();
  prefWindow->windowHandle()->raise();
  prefWindow->windowHandle()->requestActivate();
}

void SaneBreakApp::onBatterySettingChange() {
  bool doPause = preferences->pauseOnBattery->get();
  if (!doPause)
    resumeBreak(SaneBreak::PauseReason::OnBattery);
  else if (systemMonitor->isOnBattery())
    pauseBreak(SaneBreak::PauseReason::OnBattery);
}

void SaneBreakApp::confirmQuit() {
  int largestMinutes = 0;
  for (const QString &minuteString : preferences->postponeMinutes->get()) {
    int minute = minuteString.toInt();
    largestMinutes = largestMinutes > minute ? largestMinutes : minute;
  }
  QMessageBox msgBox;
  msgBox.setText(tr("Are you sure to quit Sane Break?"));
  msgBox.setInformativeText(tr("You can postpone the breaks instead."));
  msgBox.setIcon(QMessageBox::Icon::Question);
  msgBox.addButton(QMessageBox::Cancel)->setText(tr("Cancel"));
  msgBox.addButton(tr("Postpone %n min", "", largestMinutes), QMessageBox::NoRole);
  msgBox.addButton(QMessageBox::Yes)->setText(tr("Yes"));

  msgBox.setDefaultButton(QMessageBox::Cancel);
  switch (msgBox.exec()) {
    case QMessageBox::Yes:
      emit quit();
      return;
    case QMessageBox::Cancel:
      return;
    default:
      postpone(largestMinutes * 60);
      return;
  }
}
