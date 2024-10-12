// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_IDLE_H
#define SANE_BREAK_IDLE_H

#include <QObject>
#include <QTimer>

enum WatchOption { NOTIFY_FIRST_IDLE, NOTIFY_FIRST_RESUME };

class SystemIdleTime : public QObject {
  Q_OBJECT
 public:
  virtual void startWatching(WatchOption option) {};
  virtual void stopWatching() {};
  static SystemIdleTime *createIdleTimer();
  int watchAccuracy() { return m_watchAccuracy; };
  int minIdleTime() { return m_minIdleTime; };
  virtual void setWatchAccuracy(int accuracy) {};
  virtual void setMinIdleTime(int idleTime) {};
 signals:
  void idleStart();
  void idleEnd();

 protected:
  int m_watchAccuracy = 500;  // How often we watch idle time (ms)
  int m_minIdleTime = 2000;   // How long will we consider idle (ms)
};

class ReadBasedIdleTime : public SystemIdleTime {
  Q_OBJECT
 public:
  ReadBasedIdleTime();
  void startWatching(WatchOption option);
  void stopWatching();
  virtual int systemIdleTime() { return 0; };
  void setWatchAccuracy(int accuracy);
  void setMinIdleTime(int idleTime);

 private:
  QTimer *timer;
  void tick();
  bool isIdle;
};

class SleepMonitor : public QObject {
  Q_OBJECT
 public:
  SleepMonitor();
  int watchAccuracy = 5000;  // in ms
 signals:
  void sleepEnd();  // in ms
 private:
  QTimer *timer;
  int lastAwake;
  void tick();
};

#endif  // SANE_BREAK_IDLE_H
