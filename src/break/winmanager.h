#ifndef SANE_BREAK_WINDOW_MANAGER_H
#define SANE_BREAK_WINDOW_MANAGER_H

#include <QList>
#include <QObject>

#include "window.h"

class BreakWindowManager : public QObject {
  Q_OBJECT

 public:
  BreakWindowManager();
  ~BreakWindowManager();
  int remainingTime;
  void show();

 signals:
  void timeout();

 private:
  bool isIdle = true;
  bool isForceBreak = false;
  QList<BreakWindow *> windows;
  QTimer *countdownTimer;
  void createWindows();
  void tick();
  void onIdleStart();
  void onIdleEnd();
};

#endif  // SANE_BREAK_WINDOW_MANAGER_H
