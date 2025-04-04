// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_SYSTEM_MONITOR_H
#define SANE_SYSTEM_MONITOR_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include "lib/battery-status.h"
#include "lib/idle-time.h"
#include "lib/preferences.h"
#include "lib/program-monitor.h"

class AbstractSystemMonitor : public QObject {
  Q_OBJECT
 public:
  AbstractSystemMonitor(QObject *parent = nullptr) : QObject(parent) {};
  ~AbstractSystemMonitor() = default;
  virtual void start() = 0;
  virtual bool isOnBattery() = 0;

 signals:
  void idleStarted();
  void idleEnded();
  void sleepEnded();
  void batteryPowered();
  void adaptorPowered();
  void programStarted();
  void programStopped();
};

class SystemMonitor : public AbstractSystemMonitor {
  Q_OBJECT
 public:
  SystemMonitor(SanePreferences *preferences, QObject *parent = nullptr);
  ~SystemMonitor() = default;
  void start();
  bool isOnBattery();

 private:
  SanePreferences *preferences;
  SystemIdleTime *idleTimer;
  SleepMonitor *sleepMonitor;
  BatteryStatus *batteryWatcher;
  RunningProgramsMonitor *runningProgramsMonitor;
};

#endif  // SANE_SYSTEM_MONITOR_H
