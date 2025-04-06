// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

class SystemIdleTime : public QObject {
  Q_OBJECT
 public:
  SystemIdleTime(QObject *parent = nullptr) : QObject(parent) {};
  virtual void startWatching() {};
  virtual void stopWatching() {};
  static SystemIdleTime *createIdleTimer(QObject *parent = nullptr);
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
