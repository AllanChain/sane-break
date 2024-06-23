#include "scheduler.h"

#include <QObject>
#include <QSettings>
#include <QTimer>

#include "winmanager.h"

BreakScheduler::BreakScheduler() : QObject() {
  breakManager = new BreakWindowManager();
  timer = new QTimer();
  timer->setSingleShot(true);
};
BreakScheduler::~BreakScheduler(){};

void BreakScheduler::start() {
  connect(timer, &QTimer::timeout, breakManager, &BreakWindowManager::show);
  timer->start(scheduleInterval());
  connect(breakManager, &BreakWindowManager::timeout, [=]() {
    connect(timer, &QTimer::timeout, breakManager, &BreakWindowManager::show);
    timer->start(scheduleInterval());
  });
}

int BreakScheduler::remainingTime() { return timer->remainingTime(); }

int BreakScheduler::breakTime() {
  QSettings settings;
  return settings.value("break/break-time", 20 * 1000).toInt();
}

int BreakScheduler::scheduleInterval() {
  QSettings settings;
  return settings.value("break/schedule", 20 * 60 * 1000).toInt();
}
