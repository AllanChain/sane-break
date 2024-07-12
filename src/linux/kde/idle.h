#include <qglobal.h>

#ifdef Q_OS_LINUX
#ifndef SANE_IDLE_KDE_H
#define SANE_IDLE_KDE_H

#include <QTimer>

#include "idle-time.h"

int systemIdleTime();

class IdleTimeKDE : public SystemIdleTime {
  Q_OBJECT
 public:
  IdleTimeKDE();
  void startWatching();
  void stopWatching();

 private:
  bool isWatching = false;
};

#endif  // SANE_IDLE_KDE_H
#endif  // Q_OS_LINUX
