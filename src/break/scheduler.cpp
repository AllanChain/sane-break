#include "scheduler.h"

#include <QObject>
#include <QSettings>
#include <QTimer>

#include "winmanager.h"

BreakScheduler::BreakScheduler() : QObject() {
  breakManager = new BreakWindowManager();
  timer = new QTimer();
};
BreakScheduler::~BreakScheduler(){};

void BreakScheduler::start() {
  breakManager->show();
  /*timer->singleShot(nextScheduleTime(), breakManager,*/
  /*                  &BreakWindowManager::show);*/
  connect(breakManager, &BreakWindowManager::timeout, [=]() {
    timer->singleShot(nextScheduleTime(), breakManager,
                      &BreakWindowManager::show);
  });
}

int BreakScheduler::remainingTime() { return timer->remainingTime(); }

int BreakScheduler::nextBreakInterval() {
  QSettings settings;
  return settings.value("break/interval", 20 * 1000).toInt();
}

int BreakScheduler::nextScheduleTime() {
  QSettings settings;
  return settings.value("schedule/interval", 20 * 60 * 1000).toInt();
}
