// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include "core/preferences.h"
#include "core/system-monitor.h"
#include "lib/battery-status.h"
#include "lib/program-monitor.h"
#include "lib/sleep-monitor.h"

class SystemMonitor : public AbstractSystemMonitor {
  Q_OBJECT
 public:
  SystemMonitor(SanePreferences *preferences, QObject *parent = nullptr);
  ~SystemMonitor() = default;
  void start();
  bool isOnBattery();

 private:
  SanePreferences *preferences;
  SleepMonitor *sleepMonitor;
  BatteryStatus *batteryWatcher;
  RunningProgramsMonitor *runningProgramsMonitor;
};
