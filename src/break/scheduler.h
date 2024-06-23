#ifndef SANE_BREAK_SCHEDULER_H
#define SANE_BREAK_SCHEDULER_H

#include <QObject>
#include <QTimer>

#include "winmanager.h"

class BreakScheduler : public QObject {
  Q_OBJECT
 public:
  BreakScheduler();
  ~BreakScheduler();
  void start();
  int remainingTime();
  int nextScheduleTime();
  int nextBreakInterval();

 private:
  BreakWindowManager* breakManager;
  QTimer* timer;
};

#endif  // SANE_BREAK_SCHEDULER_H
