// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "system-monitor.h"

#include <QObject>

#include "core/flags.h"
#include "core/preferences.h"
#include "core/system-monitor.h"
#include "idle/factory.h"
#include "lib/battery-status.h"
#include "lib/program-monitor.h"
#include "lib/sleep-monitor.h"

SystemMonitor::SystemMonitor(SanePreferences* preferences, QObject* parent)
    : AbstractSystemMonitor(parent), preferences(preferences) {
  sleepMonitor = new SleepMonitor(this);
  batteryWatcher = BatteryStatus::createWatcher(this);
  runningProgramsMonitor = new RunningProgramsMonitor(this);

  connect(sleepMonitor, &SleepMonitor::sleepEnd, this, &SystemMonitor::sleepEnded);
  connect(batteryWatcher, &BatteryStatus::onBattery, this, [this]() {
    if (this->preferences->pauseOnBattery->get())
      emit pauseRequested(PauseReason::OnBattery);
  });
  connect(batteryWatcher, &BatteryStatus::onPower, this, [this]() {
    if (this->preferences->pauseOnBattery->get())
      emit resumeRequested(PauseReason::OnBattery);
  });
  connect(runningProgramsMonitor, &RunningProgramsMonitor::programStarted, this,
          [this]() { emit pauseRequested(PauseReason::AppOpen); });
  connect(runningProgramsMonitor, &RunningProgramsMonitor::programStopped, this,
          [this]() { emit resumeRequested(PauseReason::AppOpen); });

  connect(preferences->programsToMonitor, &SettingWithSignal::changed, this, [this]() {
    runningProgramsMonitor->setPrograms(this->preferences->programsToMonitor->get());
  });
}

void SystemMonitor::start() {
  batteryWatcher->startWatching();
  runningProgramsMonitor->setPrograms(preferences->programsToMonitor->get());
  runningProgramsMonitor->startMonitoring();
}

bool SystemMonitor::isOnBattery() { return batteryWatcher->isOnBattery; }
