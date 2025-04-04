// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <qglobal.h>
#include <qlogging.h>

#include <QAction>
#include <QDateTime>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QStyleHints>
#include <QTimer>
#include <QWindow>

#include "gui/break-window.h"
#include "gui/pref-window.h"
#include "gui/tray.h"
#include "gui/widgets/language-select.h"
#include "gui/window-manager.h"
#include "lib/app-core.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"
#include "lib/screen-lock.h"
#include "lib/system-monitor.h"
#include "lib/utils.h"

SaneBreakApp::SaneBreakApp(const AppDependencies &deps, QObject *parent)
    : AbstractApp(deps, parent) {
  prefWindow = new PreferenceWindow(preferences);
  breakManager = new BreakWindowManager(preferences, this);
  systemMonitor = new SystemMonitor(preferences, this);
  createMenu();
  tray = StatusTrayWindow::createTrayOrWindow(menu, this);

  connect(breakManager, &BreakWindowManager::resume, this,
          &SaneBreakApp::onBreakResume);
  connect(breakManager, &BreakWindowManager::timeout, this, &SaneBreakApp::onBreakEnd);
  connect(tray, &StatusTrayWindow::breakTriggered, this, &SaneBreakApp::onIconTrigger);
  connect(systemMonitor, &SystemMonitor::idleStarted, this, &SaneBreakApp::onIdleStart);
  connect(systemMonitor, &SystemMonitor::idleEnded, this, &SaneBreakApp::onIdleEnd);
  connect(systemMonitor, &SystemMonitor::sleepEnded, this, &SaneBreakApp::onSleepEnd);
  connect(systemMonitor, &SystemMonitor::batteryPowered, this,
          &SaneBreakApp::onBattery);
  connect(systemMonitor, &SystemMonitor::adaptorPowered, this, &SaneBreakApp::onPower);
  connect(systemMonitor, &SystemMonitor::programStarted, this,
          [this]() { pauseBreak(PauseReason::APP_OPEN); });
  connect(systemMonitor, &SystemMonitor::programStopped, this,
          [this]() { resumeBreak(PauseReason::APP_OPEN); });
  connect(preferences->pauseOnBattery, &SettingWithSignal::changed, this,
          &SaneBreakApp::onBatterySettingChange);
  connect(preferences->postponeMinutes, &SettingWithSignal::changed, this,
          &SaneBreakApp::onPostponeMinutesChange);
  connect(preferences->smallEvery, &SettingWithSignal::changed, this,
          &SaneBreakApp::resetSecondsToNextBreak);
  connect(preferences->language, &SettingWithSignal::changed, this,
          [this]() { LanguageSelect::setLanguage(preferences->language->get()); });
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
  breakManager->show(isBigBreak ? BreakWindow::BreakType::BIG
                                : BreakWindow::BreakType::SMALL);
}

void SaneBreakApp::closeBreakWindow() { breakManager->close(); }

void SaneBreakApp::updateTray() {
  StatusTrayWindow::IconVariants flags;
  if (pauseReasons != 0 || breakManager->isShowing() ||
      secondsToNextBreak > preferences->smallEvery->get()) {
    flags |= StatusTrayWindow::IconVariant::PAUSED;
  }
  if (smallBreaksBeforeBig() == 0) {
    flags |= StatusTrayWindow::IconVariant::WILL_BIG;
  }
  float arcRatio = float(secondsToNextBreak) / preferences->smallEvery->get();
  tray->updateIcon(arcRatio, flags);

  tray->setTitle(QString("%1 %2").arg(
      smallBreaksBeforeBig() == 0 ? tr("big break") : tr("small break"),
      formatTime(secondsToNextBreak)));
  nextBreakAction->setText(
      tr("Next break after %1").arg(formatTime(secondsToNextBreak)));
  int secondsToNextbigBreak =
      secondsToNextBreak + smallBreaksBeforeBig() * preferences->smallEvery->get();
  bigBreakAction->setText(
      tr("Next big break after %1").arg(formatTime(secondsToNextbigBreak)));

  if (!pauseReasons) {
    enableBreak->setVisible(false);
    nextBreakAction->setVisible(true);
    bigBreakAction->setVisible(true);
  } else {
    if (pauseReasons.testFlag(PauseReason::ON_BATTERY)) {
      tray->setTitle(tr("Paused on battery"));
    } else if (pauseReasons.testFlag(PauseReason::APP_OPEN)) {
      tray->setTitle(tr("Paused on app running"));
    } else if (pauseReasons.testFlag(PauseReason::IDLE)) {
      tray->setTitle(tr("Paused on idle"));
    }
    enableBreak->setVisible(true);
    nextBreakAction->setVisible(false);
    bigBreakAction->setVisible(false);
  }
}

void SaneBreakApp::onIconTrigger() {
  if (preferences->quickBreak->get()) breakNow();
}

void SaneBreakApp::createMenu() {
  menu = new QMenu();
  nextBreakAction = new QAction(tr("Next Break"), this);
  menu->addAction(nextBreakAction);
  connect(nextBreakAction, &QAction::triggered, this, [=]() { breakNow(); });

  bigBreakAction = new QAction(tr("Big Break"), this);
  menu->addAction(bigBreakAction);
  connect(bigBreakAction, &QAction::triggered, this, [=]() {
    breakCycleCount = 0;
    breakNow();
  });

  menu->addSeparator();

  postponeMenu = menu->addMenu(tr("Postpone"));
  for (const QString &minuteString : preferences->postponeMinutes->get()) {
    int minute = minuteString.toInt();
    connect(postponeMenu->addAction(tr("%n min", "", minute)), &QAction::triggered,
            this, [this, minute]() { postpone(minute * 60); });
  }
  enableBreak = menu->addAction(tr("Enable Break"));
  enableBreak->setVisible(false);
  connect(enableBreak, &QAction::triggered, this,
          // enable all flags
          [this]() { resumeBreak(PauseReasons::fromInt((1 << 8) - 1)); });

  menu->addSeparator();

  connect(menu->addAction(tr("Preferences")), &QAction::triggered, this, [this]() {
    prefWindow->show();
    prefWindow->windowHandle()->raise();
    prefWindow->windowHandle()->requestActivate();
  });

  quitAction = menu->addAction(tr("Quit"));
  connect(quitAction, &QAction::triggered, this, &SaneBreakApp::confirmQuit);
}

void SaneBreakApp::mayLockScreen() {
  if (preferences->autoScreenLock->get()) {
    if (lockScreen())
      qDebug("Screen locked");
    else
      qWarning("Failed to lock screen");
  }
}

void SaneBreakApp::onBatterySettingChange() {
  bool doPause = preferences->pauseOnBattery->get();
  if (!doPause)
    resumeBreak(PauseReason::ON_BATTERY);
  else if (systemMonitor->isOnBattery())
    pauseBreak(PauseReason::ON_BATTERY);
}

void SaneBreakApp::onPostponeMinutesChange() {
  postponeMenu->clear();
  for (const QString &minuteString : preferences->postponeMinutes->get()) {
    int minute = minuteString.toInt();
    connect(postponeMenu->addAction(tr("%n min", "", minute)), &QAction::triggered,
            this, [this, minute]() { postpone(minute * 60); });
  }
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
