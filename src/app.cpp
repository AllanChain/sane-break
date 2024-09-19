// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>

#include "battery-status.h"
#include "idle-time.h"
#include "pref-window.h"
#include "preferences.h"

SaneBreakApp::SaneBreakApp() : QObject() {
  prefWindow = new PreferenceWindow();
  breakManager = new BreakWindowManager();
  idleTimer = SystemIdleTime::createIdleTimer();
  idleTimer->watchAccuracy = 5000;
  idleTimer->minIdleTime = SanePreferences::pauseOnIdleFor() * 1000;
  sleepMonitor = new SleepMonitor();
  batteryWatcher = BatteryStatus::createWatcher();
  createMenu();
  icon = new QSystemTrayIcon(this);
  icon->setIcon(QIcon(":/images/icon.png"));
  icon->setContextMenu(menu);

  secondsToNextBreak = SanePreferences::smallEvery();
  countDownTimer = new QTimer();
  countDownTimer->setInterval(1000);
  connect(countDownTimer, &QTimer::timeout, this, &SaneBreakApp::tick);
  connect(breakManager, &BreakWindowManager::timeout, this, [this]() {
    if (pauseReasons == 0) countDownTimer->start();
  });
  connect(idleTimer, &SystemIdleTime::idleStart, this,
          [this]() { pauseBreak(PauseReason::IDLE); });
  connect(idleTimer, &SystemIdleTime::idleEnd, this, [this](int ms) {
    bool resumed = resumeBreak(PauseReason::IDLE);
    if (ms > SanePreferences::resetOnIdleFor() * 1000 && resumed) resetBreak();
  });
  connect(sleepMonitor, &SleepMonitor::sleepEnd, this,
          [this](int ms) { resetBreak(); });
  connect(batteryWatcher, &BatteryStatus::onBattery, this, [this]() {
    if (SanePreferences::pauseOnBattery()) pauseBreak(PauseReason::ON_BATTERY);
  });
  connect(batteryWatcher, &BatteryStatus::onPower, this, [this]() {
    // No need to check setitngs because it does nothing if not paused with this
    resumeBreak(PauseReason::ON_BATTERY);
  });
}
SaneBreakApp::~SaneBreakApp() {}

void SaneBreakApp::start() {
  icon->show();
  countDownTimer->start();
  idleTimer->startWatching(NOTIFY_FIRST_IDLE);
  batteryWatcher->startWatching();
}

void SaneBreakApp::tick() {
  secondsToNextBreak--;
  if (secondsToNextBreak <= 0) {
    breakNow();
    return;
  }
  if (secondsToNextBreak <= 10) {
    icon->setIcon(secondsToNextBreak % 2 == 0
                      ? QIcon(":/images/icon-yellow.png")
                      : QIcon(":/images/icon-lime.png"));
  } else if (secondsToNextBreak == 60) {
    icon->setIcon(QIcon(":/images/icon-lime.png"));
  }
  updateMenu();
}

void SaneBreakApp::updateMenu() {
  int seconds = secondsToNextBreak;
  int minutes = seconds / 60;
  seconds %= 60;
  icon->setToolTip(QString("Sane Break\n%1 break %2:%3")
                       .arg(smallBreaksBeforeBig() == 0 ? "big" : "small")
                       .arg(minutes)
                       .arg(seconds, 2, 10, QChar('0')));  // Pad zero
  nextBreakAction->setText(QString("Next break after %1:%2")
                               .arg(minutes)
                               .arg(seconds, 2, 10, QChar('0')));
  bigBreakAction->setText(
      QString("Big break after %1 breaks").arg(smallBreaksBeforeBig()));
}

void SaneBreakApp::createMenu() {
  menu = new QMenu();
  nextBreakAction = new QAction("Next Break", this);
  menu->addAction(nextBreakAction);
  connect(nextBreakAction, &QAction::triggered, this, [=]() { breakNow(); });

  bigBreakAction = new QAction("Big Break", this);
  menu->addAction(bigBreakAction);
  connect(bigBreakAction, &QAction::triggered, this, [=]() {
    breakCycleCount = 0;
    breakNow();
  });

  menu->addSeparator();

  QMenu *postponeMenu = menu->addMenu("Postpone");
  connect(postponeMenu->addAction("30 min"), &QAction::triggered, this,
          [this]() { postpone(30 * 60); });
  connect(postponeMenu->addAction("1 h"), &QAction::triggered, this,
          [this]() { postpone(60 * 60); });
  connect(postponeMenu->addAction("Quit"), &QAction::triggered, this,
          &SaneBreakApp::quit);

  menu->addSeparator();

  connect(menu->addAction("Preferences"), &QAction::triggered, this, [this]() {
    prefWindow->loadSettings();
    prefWindow->show();
  });
}

void SaneBreakApp::breakNow() {
  secondsToNextBreak = SanePreferences::smallEvery();
  updateMenu();
  countDownTimer->stop();
  breakManager->show(breakTime());
  breakCycleCount++;
  // Reset icon
  icon->setIcon(QIcon(":/images/icon.png"));
}

void SaneBreakApp::postpone(int secs) {
  secondsToNextBreak += secs;
  breakCycleCount = 0;    // break after postpone is a big break
  breakManager->close();  // stop current break if necessary
  // Reset icon
  if (secondsToNextBreak <= 60) {
    icon->setIcon(QIcon(":/images/icon-lime.png"));
  } else {
    icon->setIcon(QIcon(":/images/icon.png"));
  }
}

void SaneBreakApp::pauseBreak(PauseReason reason) {
  countDownTimer->stop();
  breakManager->close();  // stop current break if necessary
  // But the timer will resume after current break end
  // Therefore we set a flag and tell the event handler to pause
  pauseReasons |= reason;
  icon->setIcon(QIcon(":/images/icon-gray.png"));
}

// Return true if the time is running
bool SaneBreakApp::resumeBreak(PauseReason reason) {
  // Do nothing if not paused
  if (pauseReasons == 0) return true;
  // Remove specific reason for pausing
  pauseReasons &= ~reason;
  // If there are other reasons for pausing, do nothing
  if (pauseReasons != 0) return false;
  countDownTimer->start();
  icon->setIcon(QIcon(":/images/icon.png"));
  return true;
}

void SaneBreakApp::resetBreak() {
  breakCycleCount = 1;
  pauseReasons = 0;
  if (!countDownTimer->isActive()) countDownTimer->start();
  secondsToNextBreak = SanePreferences::smallEvery();
  updateMenu();
  icon->setIcon(QIcon(":/images/icon.png"));
}

int SaneBreakApp::smallBreaksBeforeBig() {
  QSettings settings;
  int breakEvery = SanePreferences::bigAfter();
  breakCycleCount %= breakEvery;
  return (breakEvery - breakCycleCount) % breakEvery;
}

int SaneBreakApp::breakTime() {
  QSettings settings;
  return (smallBreaksBeforeBig() == 0) ? SanePreferences::bigFor()
                                       : SanePreferences::smallFor();
}
