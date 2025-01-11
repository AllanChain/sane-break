// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <qglobal.h>
#include <qlogging.h>

#include <QAction>
#include <QBrush>
#include <QDateTime>
#include <QGuiApplication>
#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QRect>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QStyleHints>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWindow>

#include "battery-status.h"
#include "idle-time.h"
#include "pref-window.h"
#include "preferences.h"
#include "program-monitor.h"
#include "screen-lock.h"
#include "window-manager.h"

SaneBreakApp::SaneBreakApp() : QObject() {
  prefWindow = new PreferenceWindow();
  breakManager = new BreakWindowManager();
  idleTimer = SystemIdleTime::createIdleTimer();
  idleTimer->setWatchAccuracy(5000);
  idleTimer->setMinIdleTime(SanePreferences::pauseOnIdleFor->get() * 1000);
  // One-shot idle detection right after break end is achieved by making
  // the idle criteria as short as 1 sec, and start the idle timer right
  // after the break starts. We will know if the user is idle after breaks
  // by checking the idle status right after the break.
  // This timer will be deactivated at the first activity after the break.
  oneshotIdleTimer = SystemIdleTime::createIdleTimer();
  oneshotIdleTimer->setWatchAccuracy(1000);
  oneshotIdleTimer->setMinIdleTime(1000);
  screenLockTimer = new QTimer();
  screenLockTimer->setSingleShot(true);
  sleepMonitor = new SleepMonitor();
  batteryWatcher = BatteryStatus::createWatcher();
  runningProgramsMonitor = new RunningProgramsMonitor();
  createMenu();
  icon = new QSystemTrayIcon(this);
  icon->setContextMenu(menu);

  countDownTimer = new QTimer();
  countDownTimer->setInterval(1000);
  connect(countDownTimer, &QTimer::timeout, this, &SaneBreakApp::tick);
  connect(breakManager, &BreakWindowManager::resume, this,
          &SaneBreakApp::onBreakResume);
  connect(breakManager, &BreakWindowManager::timeout, this, &SaneBreakApp::onBreakEnd);
  connect(icon, &QSystemTrayIcon::activated, this, &SaneBreakApp::onIconTrigger);
  connect(idleTimer, &SystemIdleTime::idleStart, this, &SaneBreakApp::onIdleStart);
  connect(idleTimer, &SystemIdleTime::idleEnd, this, &SaneBreakApp::onIdleEnd);
  connect(oneshotIdleTimer, &SystemIdleTime::idleEnd, this,
          &SaneBreakApp::onOneshotIdleEnd);
  connect(screenLockTimer, &QTimer::timeout, this, &SaneBreakApp::mayLockScreen);
  connect(sleepMonitor, &SleepMonitor::sleepEnd, this, &SaneBreakApp::onSleepEnd);
  connect(batteryWatcher, &BatteryStatus::onBattery, this, &SaneBreakApp::onBattery);
  connect(batteryWatcher, &BatteryStatus::onPower, this, &SaneBreakApp::onPower);
  connect(runningProgramsMonitor, &RunningProgramsMonitor::programStarted, this,
          [this]() { pauseBreak(PauseReason::APP_OPEN); });
  connect(runningProgramsMonitor, &RunningProgramsMonitor::programStopped, this,
          [this]() { resumeBreak(PauseReason::APP_OPEN); });
  connect(SanePreferences::pauseOnBattery, &SettingWithSignal::changed, this,
          &SaneBreakApp::onBatterySettingChange);
  connect(SanePreferences::smallEvery, &SettingWithSignal::changed, this,
          &SaneBreakApp::resetSecondsToNextBreak);
  connect(
      SanePreferences::programsToMonitor, &SettingWithSignal::changed, this, [this]() {
        runningProgramsMonitor->setPrograms(SanePreferences::programsToMonitor->get());
      });
}

SaneBreakApp::~SaneBreakApp() {}

void SaneBreakApp::start() {
  resetSecondsToNextBreak();
  icon->show();
  countDownTimer->start();
  idleTimer->startWatching();
  batteryWatcher->startWatching();
  runningProgramsMonitor->setPrograms(SanePreferences::programsToMonitor->get());
  runningProgramsMonitor->startMonitoring();
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

  if (smallBreaksBeforeBig() == 0) {
    int dotSize = 14;
    QRect smallRect(pixmap.width() - 1 - dotSize, 1, dotSize, dotSize);
    QBrush brush(QColor(202, 138, 4, 255));
    painter.setBrush(brush);
    pen.setWidth(0);
    painter.setPen(pen);
    painter.drawEllipse(smallRect);
  }

  icon->setIcon(pixmap);
}

void SaneBreakApp::onIconTrigger(QSystemTrayIcon::ActivationReason reason) {
  if (SanePreferences::quickBreak->get() && (reason == QSystemTrayIcon::DoubleClick ||
                                             reason == QSystemTrayIcon::MiddleClick)) {
    breakNow();
  }
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
    prefWindow->windowHandle()->raise();
    prefWindow->windowHandle()->requestActivate();
  });
}

void SaneBreakApp::breakNow() {
  resetSecondsToNextBreak();
  updateMenu();
  countDownTimer->stop();
  breakManager->show(smallBreaksBeforeBig() == 0 ? BreakType::BIG : BreakType::SMALL);
  breakCycleCount++;
  // For testing user is idle after break end
  oneshotIdleTimer->startWatching();
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

void SaneBreakApp::pauseBreak(unsigned int reason) {
  // Should not record last pause if already paused
  if (pauseReasons == 0) lastPause = QDateTime::currentSecsSinceEpoch();
  pauseReasons |= reason;  // Flag should be set before closing windows
  countDownTimer->stop();
  // Stop current break if necessary
  if (reason != PauseReason::IDLE) breakManager->close();
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
bool SaneBreakApp::resumeBreak(unsigned int reason) {
  // Do nothing if not paused
  if (pauseReasons == 0) return true;
  // Remove specific reason for pausing
  pauseReasons &= ~reason;
  // If there are other reasons for pausing, do nothing
  if (pauseReasons != 0) return false;

  if (lastPause > 0) {  // We have correctly recorded last pause time
    int secPaused = QDateTime::currentSecsSinceEpoch() - lastPause;
    lastPause = 0;
    if (secPaused > SanePreferences::resetAfterPause->get()) resetSecondsToNextBreak();
    if (secPaused > SanePreferences::resetCycleAfterPause->get()) breakCycleCount = 1;
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
  // But we update menu and icon (in case <1min) only if not paused
  if (pauseReasons == 0) {
    updateMenu();
    updateIcon();
  }
}

void SaneBreakApp::onBreakResume() {
  int msec = SanePreferences::autoScreenLock->get() * 1000;
  if (msec == 0 && screenLockTimer->isActive())
    screenLockTimer->stop();
  else
    screenLockTimer->start(msec);
}

// Resume countdown if user is idle after breaks
void SaneBreakApp::onBreakEnd() {
  if (!oneshotIdleTimer->isIdle) {
    oneshotIdleTimer->stopWatching();
    screenLockTimer->stop();
    // Continue countdown as normal
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

void SaneBreakApp::mayLockScreen() {
  if (SanePreferences::autoScreenLock->get()) {
    if (lockScreen())
      qDebug("Screen locked");
    else
      qWarning("Failed to lock screen");
  }
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
