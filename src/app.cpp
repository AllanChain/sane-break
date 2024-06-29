#include "app.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTimer>

SaneBreakApp::SaneBreakApp() : QObject() {
  breakManager = new BreakWindowManager();
  createMenu();
  icon = new QSystemTrayIcon(this);
  icon->setIcon(QIcon(":/images/icon.png"));
  icon->setContextMenu(menu);

  secondsToNextBreak = scheduleInterval();
  countDownTimer = new QTimer();
  countDownTimer->setInterval(1000);
  connect(countDownTimer, &QTimer::timeout, this, &SaneBreakApp::tick);
  connect(breakManager, &BreakWindowManager::timeout,
          [this]() { countDownTimer->start(); });
}
SaneBreakApp::~SaneBreakApp() {}

void SaneBreakApp::start() {
  icon->show();
  countDownTimer->start();
}

void SaneBreakApp::tick() {
  secondsToNextBreak--;
  if (secondsToNextBreak <= 0) {
    secondsToNextBreak = scheduleInterval();
    countDownTimer->stop();
    breakManager->show();
  }
  int seconds = secondsToNextBreak;
  int minutes = seconds / 60;
  seconds %= 60;
  if (minutes > 0) {
    nextBreakAction->setText(
        QString("Next break %1 min %2 secs").arg(minutes).arg(seconds));
  } else {
    nextBreakAction->setText(QString("Next break %1 secs").arg(seconds));
  }
}

void SaneBreakApp::createMenu() {
  menu = new QMenu();
  nextBreakAction = new QAction("Next Break", this);
  nextBreakAction->setDisabled(true);
  menu->addAction(nextBreakAction);

  breakNowAction = new QAction("Break Now", this);
  menu->addAction(breakNowAction);
  connect(breakNowAction, &QAction::triggered, this, &SaneBreakApp::breakNow);

  quitAction = new QAction("Quit", this);
  menu->addAction(quitAction);
  connect(quitAction, &QAction::triggered, this, &SaneBreakApp::quit);
}

void SaneBreakApp::breakNow() {
  secondsToNextBreak = scheduleInterval();
  countDownTimer->stop();
  breakManager->show();
}

int SaneBreakApp::breakTime() {
  QSettings settings;
  return settings.value("break/break-time", 20).toInt();
}

int SaneBreakApp::scheduleInterval() {
  QSettings settings;
  return settings.value("break/schedule", 20 * 60).toInt();
}
