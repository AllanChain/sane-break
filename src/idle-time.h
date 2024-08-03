// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_IDLE_H
#define SANE_BREAK_IDLE_H

#include <QElapsedTimer>
#include <QObject>
#include <QTimer>

enum WatchOption { NOTIFY_FIRST_IDLE, NOTIFY_FIRST_RESUME };

class SystemIdleTime : public QObject {
  Q_OBJECT
 public:
  virtual void startWatching(WatchOption option) {};
  virtual void stopWatching() {};
  int watchAccuracy = 500;  // How often we watch idle time (ms)
  int minIdleTime = 2000;   // How long will we consider idle (ms)
  static SystemIdleTime *createIdleTimer();
 signals:
  void idleStart();
  void idleEnd(int ms);
};

class SleepMonitor : public QObject {
  Q_OBJECT
 public:
  SleepMonitor();
  int watchAccuracy = 5000;  // in ms
 signals:
  void sleepEnd(int sleepTime);  // in ms
 private:
  QTimer *timer;
  QElapsedTimer *elapsedTimer;
  void tick();
};

#endif  // SANE_BREAK_IDLE_H
