// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_IDLE_H
#define SANE_BREAK_IDLE_H

#include <QObject>
#include <QTimer>

class SystemIdleTime : public QObject {
  Q_OBJECT
 public:
  virtual void startWatching() {};
  virtual void stopWatching() {};
  static SystemIdleTime *createIdleTimer();
  int watchAccuracy() { return m_watchAccuracy; };
  int minIdleTime() { return m_minIdleTime; };
  virtual void setWatchAccuracy(int accuracy) {};
  virtual void setMinIdleTime(int idleTime) {};
  bool isIdle = false;
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
  void startWatching();
  void stopWatching();
  virtual int systemIdleTime() { return 0; };
  void setWatchAccuracy(int accuracy);
  void setMinIdleTime(int idleTime);

 private:
  QTimer *timer;
  QTimer *delay;
  void tick();
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
