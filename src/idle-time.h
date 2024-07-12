#ifndef SANE_BREAK_IDLE_H
#define SANE_BREAK_IDLE_H

#include <QObject>
#include <QTimer>

class SystemIdleTime : public QObject {
  Q_OBJECT
 public:
  virtual void startWatching() {};
  virtual void stopWatching() {};
  static SystemIdleTime* createIdleTimer();
 signals:
  void idleStart();
  void idleEnd();
};

#endif  // SANE_BREAK_IDLE_H
