// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "system-monitor.h"

#include <QObject>

#include "lib/battery-status.h"
#include "lib/program-monitor.h"
#include "lib/sleep-monitor.h"

SystemMonitor::SystemMonitor(SanePreferences* preferences, QObject* parent)
    : AbstractSystemMonitor(parent), preferences(preferences) {
  idleTimer = SystemIdleTime::createIdleTimer(this);
  idleTimer->setWatchAccuracy(5000);
  idleTimer->setMinIdleTime(preferences->pauseOnIdleFor->get() * 1000);
  sleepMonitor = new SleepMonitor(this);
  batteryWatcher = BatteryStatus::createWatcher(this);
  runningProgramsMonitor = new RunningProgramsMonitor(this);

  connect(idleTimer, &SystemIdleTime::idleStart, this, &SystemMonitor::idleStarted);
  connect(idleTimer, &SystemIdleTime::idleEnd, this, &SystemMonitor::idleEnded);
  connect(sleepMonitor, &SleepMonitor::sleepEnd, this, &SystemMonitor::sleepEnded);
  connect(batteryWatcher, &BatteryStatus::onBattery, this,
          &SystemMonitor::batteryPowered);
  connect(batteryWatcher, &BatteryStatus::onPower, this,
          &SystemMonitor::adaptorPowered);
  connect(runningProgramsMonitor, &RunningProgramsMonitor::programStarted, this,
          &SystemMonitor::programStarted);
  connect(runningProgramsMonitor, &RunningProgramsMonitor::programStopped, this,
          &SystemMonitor::programStopped);

  connect(preferences->programsToMonitor, &SettingWithSignal::changed, this, [this]() {
    runningProgramsMonitor->setPrograms(this->preferences->programsToMonitor->get());
  });
}

void SystemMonitor::start() {
  idleTimer->startWatching();
  batteryWatcher->startWatching();
  runningProgramsMonitor->setPrograms(preferences->programsToMonitor->get());
  runningProgramsMonitor->startMonitoring();
}

bool SystemMonitor::isOnBattery() { return batteryWatcher->isOnBattery; }
