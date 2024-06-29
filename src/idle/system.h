#ifndef SANE_IDLE_SYSTEM_H
#define SANE_IDLE_SYSTEM_H

#include <QObject>
#include <QTimer>

class SystemIdleTime : public QObject {
  Q_OBJECT
 public:
  virtual void startWatching(){};
  virtual void stopWatching(){};
  static SystemIdleTime* createIdleTimer();
 signals:
  void idleStart();
  void idleEnd();
};

#endif  // SANE_IDLE_SYSTEM_H
