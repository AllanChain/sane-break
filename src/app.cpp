// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <QAction>
#include <QDateTime>
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
  idleTimer->minIdleTime = SanePreferences::pauseOnIdleFor->get() * 1000;
  sleepMonitor = new SleepMonitor();
  batteryWatcher = BatteryStatus::createWatcher();
  createMenu();
  icon = new QSystemTrayIcon(this);
  icon->setIcon(QIcon(":/images/icon.png"));
  icon->setContextMenu(menu);

  secondsToNextBreak = SanePreferences::smallEvery->get();
  countDownTimer = new QTimer();
  countDownTimer->setInterval(1000);
  connect(countDownTimer, &QTimer::timeout, this, &SaneBreakApp::tick);
  connect(breakManager, &BreakWindowManager::timeout, this, [this]() {
    if (pauseReasons == 0) countDownTimer->start();
  });
  connect(idleTimer, &SystemIdleTime::idleStart, this,
          [this]() { pauseBreak(PauseReason::IDLE); });
  connect(idleTimer, &SystemIdleTime::idleEnd, this,
          [this]() { bool resumed = resumeBreak(PauseReason::IDLE); });
  connect(sleepMonitor, &SleepMonitor::sleepEnd, this,
          &SaneBreakApp::onSleepEnd);
  connect(batteryWatcher, &BatteryStatus::onBattery, this, [this]() {
    if (SanePreferences::pauseOnBattery->get())
      pauseBreak(PauseReason::ON_BATTERY);
  });
  connect(batteryWatcher, &BatteryStatus::onPower, this, [this]() {
    // No need to check setitngs because it does nothing if not paused with this
    resumeBreak(PauseReason::ON_BATTERY);
  });
  connect(SanePreferences::pauseOnBattery, &SettingWithSignal::changed, this,
          [this]() {
            bool doPause = SanePreferences::pauseOnBattery->get();
            if (!doPause)
              resumeBreak(PauseReason::ON_BATTERY);
            else if (batteryWatcher->isOnBattery)
              pauseBreak(PauseReason::ON_BATTERY);
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
  enableBreak = menu->addAction("Enable Break");
  enableBreak->setVisible(false);
  connect(enableBreak, &QAction::triggered, this,
          // enable all flags
          [this]() { resumeBreak((1 << 8) - 1); });

  menu->addSeparator();

  connect(menu->addAction("Preferences"), &QAction::triggered, this, [this]() {
    prefWindow->loadSettings();
    prefWindow->show();
  });
}

void SaneBreakApp::breakNow() {
  secondsToNextBreak = SanePreferences::smallEvery->get();
  updateMenu();
  countDownTimer->stop();
  breakManager->show(smallBreaksBeforeBig() == 0 ? BreakType::BIG
                                                 : BreakType::SMALL);
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

void SaneBreakApp::pauseBreak(uint reason) {
  // Should not record last pause if already paused
  if (pauseReasons == 0) lastPause = QDateTime::currentSecsSinceEpoch();
  pauseReasons |= reason;  // Flag should be set before closing windows
  countDownTimer->stop();
  breakManager->close();  // stop current break if necessary
  if (reason & PauseReason::ON_BATTERY) {
    icon->setToolTip("Sane Break\nPaused on battery");
  } else if (reason & PauseReason::APP_OPEN) {
    icon->setToolTip("Sane Break\nPaused on app running");
  } else if (reason & PauseReason::IDLE) {
    icon->setToolTip("Sane Break\nPaused on idle");
  }
  enableBreak->setVisible(true);
  nextBreakAction->setVisible(false);
  bigBreakAction->setVisible(false);
  icon->setIcon(QIcon(":/images/icon-gray.png"));
}

// Return true if the time is running
bool SaneBreakApp::resumeBreak(uint reason) {
  // Do nothing if not paused
  if (pauseReasons == 0) return true;
  // Remove specific reason for pausing
  pauseReasons &= ~reason;
  // If there are other reasons for pausing, do nothing
  if (pauseReasons != 0) return false;

  if (lastPause > 0) {  // We have correctly recorded last pause time
    int secPaused = QDateTime::currentSecsSinceEpoch() - lastPause;
    lastPause = 0;
    // "self healing" algorithm for break time
    // We don't want to have a break soon after breaks resumed.
    // Thus we add a little bit time according to the time paused.
    secondsToNextBreak = secondsToNextBreak + secPaused;
    if (secondsToNextBreak > SanePreferences::smallEvery->get())
      secondsToNextBreak = SanePreferences::smallEvery->get();
  }
  countDownTimer->start();

  enableBreak->setVisible(false);
  nextBreakAction->setVisible(true);
  bigBreakAction->setVisible(true);
  icon->setIcon(QIcon(":/images/icon.png"));
  return true;
}

void SaneBreakApp::onSleepEnd() {
  // We reset these regardless of paused or not
  breakCycleCount = 1;
  breakManager->close();  // stop current break if necessary
  secondsToNextBreak = SanePreferences::smallEvery->get();
  updateMenu();
  // Bue we update icon (in case <1min) only if not paused
  if (pauseReasons == 0) icon->setIcon(QIcon(":/images/icon.png"));
}

int SaneBreakApp::smallBreaksBeforeBig() {
  QSettings settings;
  int breakEvery = SanePreferences::bigAfter->get();
  breakCycleCount %= breakEvery;
  return (breakEvery - breakCycleCount) % breakEvery;
}
