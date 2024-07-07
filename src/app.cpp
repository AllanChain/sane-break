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
    breakNow();
    icon->setIcon(QIcon(":/images/icon.png"));
    return;
  }
  if (secondsToNextBreak <= 10) {
    icon->setIcon(secondsToNextBreak % 2 == 0
                      ? QIcon(":/images/icon-yellow.png")
                      : QIcon(":/images/icon-lime.png"));
  } else if (secondsToNextBreak == 60) {
    icon->setIcon(QIcon(":/images/icon-lime.png"));
  }
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
  connect(nextBreakAction, &QAction::triggered, [=]() { breakNow(); });

  bigBreakAction = new QAction("Big Break", this);
  menu->addAction(bigBreakAction);
  connect(bigBreakAction, &QAction::triggered, [=]() {
    breakCycleCount = 0;
    breakNow();
  });

  menu->addSeparator();

  quitAction = new QAction("Quit", this);
  menu->addAction(quitAction);
  connect(quitAction, &QAction::triggered, this, &SaneBreakApp::quit);
}

void SaneBreakApp::breakNow() {
  secondsToNextBreak = scheduleInterval();
  countDownTimer->stop();
  breakManager->show(breakTime());
  breakCycleCount++;
}

int SaneBreakApp::smallBreaksBeforeBig() {
  QSettings settings;
  int breakEvery = settings.value("break/big-break-every", 3).toInt();
  breakCycleCount %= breakEvery;
  return (breakEvery - breakCycleCount) % breakEvery;
}

int SaneBreakApp::breakTime() {
  QSettings settings;
  if (smallBreaksBeforeBig() == 0)
    return settings.value("break/big-break-time", 60).toInt();
  else
    return settings.value("break/small-break-time", 20).toInt();
}

int SaneBreakApp::scheduleInterval() {
  QSettings settings;
  return settings.value("break/schedule", 20 * 60).toInt();
}
