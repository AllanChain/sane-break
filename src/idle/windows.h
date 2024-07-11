#include <qglobal.h>
#ifdef Q_OS_WIN
#ifndef SANE_IDLE_WINDOWS_H
#define SANE_IDLE_WINDOWS_H

#include <QTimer>

#include "system.h"

int systemIdleTime();

class IdleTimeWindows : public SystemIdleTime {
  Q_OBJECT
 public:
  IdleTimeWindows();
  void startWatching();
  void stopWatching();

 private:
  QTimer* timer;
  void tick();
  bool isIdle;
};

#endif  // SANE_IDLE_WINDOWS_H
#endif  // Q_OS_WIN
