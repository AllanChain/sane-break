// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <qglobal.h>

#include <QAction>
#include <QDateTime>
#include <QGuiApplication>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QSettings>
#include <QStyleHints>
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
  idleTimer->setWatchAccuracy(5000);
  idleTimer->setMinIdleTime(SanePreferences::pauseOnIdleFor->get() * 1000);
  // One-shot idle detection right after break end
  oneshotIdleTimer = SystemIdleTime::createIdleTimer();
  oneshotIdleTimer->setWatchAccuracy(1000);
  oneshotIdleTimer->setMinIdleTime(1000);
  sleepMonitor = new SleepMonitor();
  batteryWatcher = BatteryStatus::createWatcher();
  createMenu();
  icon = new QSystemTrayIcon(this);
  icon->setContextMenu(menu);

  countDownTimer = new QTimer();
  countDownTimer->setInterval(1000);
  connect(countDownTimer, &QTimer::timeout, this, &SaneBreakApp::tick);
  connect(breakManager, &BreakWindowManager::timeout, this, &SaneBreakApp::onBreakEnd);
  connect(idleTimer, &SystemIdleTime::idleStart, this, &SaneBreakApp::onIdleStart);
  connect(idleTimer, &SystemIdleTime::idleEnd, this, &SaneBreakApp::onIdleEnd);
  connect(oneshotIdleTimer, &SystemIdleTime::idleEnd, this,
          &SaneBreakApp::onOneshotIdleEnd);
  connect(sleepMonitor, &SleepMonitor::sleepEnd, this, &SaneBreakApp::onSleepEnd);
  connect(batteryWatcher, &BatteryStatus::onBattery, this, &SaneBreakApp::onBattery);
  connect(batteryWatcher, &BatteryStatus::onPower, this, &SaneBreakApp::onPower);
  connect(SanePreferences::pauseOnBattery, &SettingWithSignal::changed, this,
          &SaneBreakApp::onBatterySettingChange);
  connect(SanePreferences::smallEvery, &SettingWithSignal::changed, this,
          &SaneBreakApp::resetSecondsToNextBreak);
}
SaneBreakApp::~SaneBreakApp() {}

void SaneBreakApp::start() {
  resetSecondsToNextBreak();
  icon->show();
  countDownTimer->start();
  idleTimer->startWatching(NOTIFY_FIRST_IDLE);
  batteryWatcher->startWatching();
}

void SaneBreakApp::tick() {
  addSecondsToNextBreak(-1);
  if (secondsToNextBreak <= 0) {
    breakNow();
    return;
  }
  updateMenu();
}

void SaneBreakApp::updateIcon() {
  if (pauseReasons != 0 || breakManager->isShowing() ||
      secondsToNextBreak > SanePreferences::smallEvery->get())
    return icon->setIcon(QIcon(":/images/icon_tray-pause.png"));

  QPixmap pixmap(":/images/icon_tray.png");
  QPen pen(QColor(5, 46, 22, 255));
  QPainter painter(&pixmap);

  painter.setRenderHint(QPainter::Antialiasing, true);
  pen.setWidth(3);
  painter.setPen(pen);
  QRect rect = pixmap.rect();
  rect.setSize(QSize(rect.width() - 2, rect.height() - 2));
  rect.setTopLeft(QPoint(2, 2));
  // Draw dark circle background
  painter.drawArc(rect, 0, 360 * 16);  // angles are in 1/16th of a degree

  // Draw light tracks
  pen.setColor(QColor(220, 252, 231, 255));
  painter.setPen(pen);
  int spanAngle = 360 * 16 * secondsToNextBreak / SanePreferences::smallEvery->get();
  painter.drawArc(rect, 90 * 16, spanAngle);

  icon->setIcon(pixmap);
}

void SaneBreakApp::addSecondsToNextBreak(int seconds) {
  secondsToNextBreak += seconds;
  updateIcon();
}

void SaneBreakApp::resetSecondsToNextBreak() {
  secondsToNextBreak = SanePreferences::smallEvery->get();
  updateIcon();
}

void SaneBreakApp::updateMenu() {
  int seconds = secondsToNextBreak;
  int minutes = seconds / 60;
  seconds %= 60;
  icon->setToolTip(QString("Sane Break\n%1 break %2:%3")
                       .arg(smallBreaksBeforeBig() == 0 ? "big" : "small")
                       .arg(minutes)
                       .arg(seconds, 2, 10, QChar('0')));  // Pad zero
  nextBreakAction->setText(
      QString("Next break after %1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));
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
  connect(postponeMenu->addAction("5 min"), &QAction::triggered, this,
          [this]() { postpone(5 * 60); });
  connect(postponeMenu->addAction("10 min"), &QAction::triggered, this,
          [this]() { postpone(10 * 60); });
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
  resetSecondsToNextBreak();
  updateMenu();
  countDownTimer->stop();
  breakManager->show(smallBreaksBeforeBig() == 0 ? BreakType::BIG : BreakType::SMALL);
  breakCycleCount++;
  // For testing user is idle after break end
  oneshotIdleTimer->startWatching(NOTIFY_FIRST_IDLE);
  // Reset icon
  updateIcon();
}

void SaneBreakApp::postpone(int secs) {
  // Postpone adds time from 0 instead of break interval
  if (breakManager->isShowing()) secondsToNextBreak = 0;
  addSecondsToNextBreak(secs);
  breakCycleCount = 0;    // break after postpone is a big break
  breakManager->close();  // stop current break if necessary
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
  updateIcon();
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
    if (secondsToNextBreak + secPaused > SanePreferences::smallEvery->get()) {
      resetSecondsToNextBreak();
      if (secPaused > SanePreferences::smallEvery->get()) {
        breakCycleCount = 1;  // reset cycle of idle for a long time
      }
    } else {
      addSecondsToNextBreak(secPaused);
    }
  }
  countDownTimer->start();

  enableBreak->setVisible(false);
  nextBreakAction->setVisible(true);
  bigBreakAction->setVisible(true);
  updateIcon();
  return true;
}

int SaneBreakApp::smallBreaksBeforeBig() {
  int breakEvery = SanePreferences::bigAfter->get();
  breakCycleCount %= breakEvery;
  return (breakEvery - breakCycleCount) % breakEvery;
}

void SaneBreakApp::onSleepEnd() {
  // We reset these regardless of paused or not
  breakCycleCount = 1;
  breakManager->close();  // stop current break if necessary
  resetSecondsToNextBreak();
  // Bue we update menu and icon (in case <1min) only if not paused
  if (pauseReasons == 0) {
    updateMenu();
    updateIcon();
  }
}

void SaneBreakApp::onBreakEnd() {
  if (!oneshotIdleTimer->isIdle) {
    // Continue countdown as normal
    oneshotIdleTimer->stopWatching();
    countDownTimer->start();
  } else {
    pauseBreak(PauseReason::IDLE);
  }
}

void SaneBreakApp::onIdleStart() { pauseBreak(PauseReason::IDLE); }

void SaneBreakApp::onIdleEnd() { bool resumed = resumeBreak(PauseReason::IDLE); }

void SaneBreakApp::onOneshotIdleEnd() {
  if (breakManager->isShowing()) return;
  oneshotIdleTimer->stopWatching();
  resumeBreak(PauseReason::IDLE);
}

void SaneBreakApp::onBattery() {
  if (SanePreferences::pauseOnBattery->get()) pauseBreak(PauseReason::ON_BATTERY);
}

void SaneBreakApp::onPower() {
  // No need to check settings because it does nothing if not paused with this
  resumeBreak(PauseReason::ON_BATTERY);
}

void SaneBreakApp::onBatterySettingChange() {
  bool doPause = SanePreferences::pauseOnBattery->get();
  if (!doPause)
    resumeBreak(PauseReason::ON_BATTERY);
  else if (batteryWatcher->isOnBattery)
    pauseBreak(PauseReason::ON_BATTERY);
}
