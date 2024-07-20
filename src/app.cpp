// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>

#include "default-pref.h"
#include "idle-time.h"
#include "pref-window.h"

SaneBreakApp::SaneBreakApp() : QObject() {
  prefWindow = new PreferenceWindow();
  breakManager = new BreakWindowManager();
  idleTimer = SystemIdleTime::createIdleTimer();
  idleTimer->watchAccuracy = 5000;
  idleTimer->minIdleTime = pauseOnIdleFor() * 1000;
  createMenu();
  icon = new QSystemTrayIcon(this);
  icon->setIcon(QIcon(":/images/icon.png"));
  icon->setContextMenu(menu);

  secondsToNextBreak = scheduleInterval();
  countDownTimer = new QTimer();
  countDownTimer->setInterval(1000);
  connect(countDownTimer, &QTimer::timeout, this, &SaneBreakApp::tick);
  connect(breakManager, &BreakWindowManager::timeout, this, [this]() {
    if (!inPause) countDownTimer->start();
  });
  connect(idleTimer, &SystemIdleTime::idleStart, this,
          &SaneBreakApp::pauseBreak);
  connect(idleTimer, &SystemIdleTime::idleEnd, this,
          &SaneBreakApp::resumeBreak);
}
SaneBreakApp::~SaneBreakApp() {}

void SaneBreakApp::start() {
  icon->show();
  countDownTimer->start();
  idleTimer->startWatching(NOTIFY_FIRST_IDLE);
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
  secondsToNextBreak = scheduleInterval();
  updateMenu();
  countDownTimer->stop();
  breakManager->show(breakTime());
  breakCycleCount++;
  // Reset icon
  icon->setIcon(QIcon(":/images/icon.png"));
}

void SaneBreakApp::postpone(int secs) {
  secondsToNextBreak += secs;
  // Break after postpone is a big break
  breakCycleCount = 0;
  // Stop current break if necessary
  if (!countDownTimer->isActive()) breakManager->close();
  // Reset icon
  if (secondsToNextBreak <= 60) {
    icon->setIcon(QIcon(":/images/icon-lime.png"));
  } else {
    icon->setIcon(QIcon(":/images/icon.png"));
  }
}

void SaneBreakApp::pauseBreak() {
  // Break after pause and resume is a small break
  breakCycleCount = 1;
  if (countDownTimer->isActive())
    countDownTimer->stop();
  else  // Stop current break if necessary
    breakManager->close();
  // But the timer will resume after current break end
  // Therefore we set a flag and tell the event handler to pause
  inPause = true;
  // Reset remaining time
  secondsToNextBreak = scheduleInterval();
}

void SaneBreakApp::resumeBreak() {
  inPause = false;
  countDownTimer->start();
}

int SaneBreakApp::smallBreaksBeforeBig() {
  QSettings settings;
  int breakEvery =
      settings.value("break/big-after", SANE_BREAK_BIG_AFTER).toInt();
  breakCycleCount %= breakEvery;
  return (breakEvery - breakCycleCount) % breakEvery;
}

int SaneBreakApp::breakTime() {
  QSettings settings;
  if (smallBreaksBeforeBig() == 0)
    return settings.value("break/big-for", SANE_BREAK_BIG_FOR).toInt();
  else
    return settings.value("break/small-for", SANE_BREAK_SMALL_FOR).toInt();
}

int SaneBreakApp::scheduleInterval() {
  QSettings settings;
  return settings.value("break/small-every", SANE_BREAK_SMALL_EVERY).toInt();
}

int SaneBreakApp::pauseOnIdleFor() {
  QSettings settings;
  return settings.value("break/pause-on-idle", SANE_BREAK_PAUSE_ON_IDLE_FOR)
      .toInt();
}
