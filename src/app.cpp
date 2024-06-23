#include "app.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>

#include "break/scheduler.h"

SaneBreakApp::SaneBreakApp() : QObject() {
  breakScheduler = new BreakScheduler();
  createMenu();
  icon = new QSystemTrayIcon(this);
  icon->setIcon(QIcon(":/images/icon.png"));
  icon->setContextMenu(menu);

  updateTimer = new QTimer();
  updateTimer->setTimerType(Qt::PreciseTimer);
  connect(updateTimer, &QTimer::timeout, this,
          &SaneBreakApp::updateNextBreakTime);
}
SaneBreakApp::~SaneBreakApp() {}

void SaneBreakApp::start() {
  icon->show();
  breakScheduler->start();
}

void SaneBreakApp::createMenu() {
  menu = new QMenu();
  nextBreakAction = new QAction("Next Break", this);
  nextBreakAction->setDisabled(true);
  menu->addAction(nextBreakAction);
  quitAction = new QAction("Quit", this);
  menu->addAction(quitAction);
  connect(quitAction, &QAction::triggered, this, &SaneBreakApp::quit);
  connect(menu, &QMenu::aboutToShow, this, [=]() {
    updateNextBreakTime();
    updateTimer->start(1000);
  });
  connect(menu, &QMenu::aboutToHide, this, [=]() { updateTimer->stop(); });
}

void SaneBreakApp::updateNextBreakTime() {
  int remainingTime = breakScheduler->remainingTime();
  int seconds = remainingTime / 1000;
  int minutes = seconds / 60;
  seconds %= 60;
  if (minutes > 0) {
    nextBreakAction->setText(
        QString("Next break %1 min %2 secs").arg(minutes).arg(seconds));
  } else {
    nextBreakAction->setText(QString("Next break %1 secs").arg(seconds));
  }
}
