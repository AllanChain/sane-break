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
#include "lib/screen-monitor.h"
#include "lib/sleep-monitor.h"

SystemMonitor::SystemMonitor(SanePreferences* preferences, QObject* parent)
    : AbstractSystemMonitor(parent), preferences(preferences) {
  sleepMonitor = new SleepMonitor(this);
  batteryWatcher = BatteryStatus::createWatcher(this);
  runningProgramsMonitor = new RunningProgramsMonitor(this);
  screenMonitor = new ScreenMonitor(this);

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

  connect(screenMonitor, &ScreenMonitor::unknownMonitorConnected, this, [this]() {
    if (this->preferences->pauseOnUnknownMonitor->get())
      emit pauseRequested(PauseReason::UnknownMonitor);
  });
  connect(screenMonitor, &ScreenMonitor::allMonitorsKnown, this,
          [this]() { emit resumeRequested(PauseReason::UnknownMonitor); });
  connect(preferences->knownMonitors, &SettingWithSignal::changed, this, [this]() {
    screenMonitor->setKnownMonitors(this->preferences->knownMonitors->get());
  });
  connect(preferences->pauseOnUnknownMonitor, &SettingWithSignal::changed, this,
          [this]() {
            if (!this->preferences->pauseOnUnknownMonitor->get())
              emit resumeRequested(PauseReason::UnknownMonitor);
          });
}

void SystemMonitor::start() {
  batteryWatcher->startWatching();
  runningProgramsMonitor->setPrograms(preferences->programsToMonitor->get());
  runningProgramsMonitor->startMonitoring();
  screenMonitor->setKnownMonitors(preferences->knownMonitors->get());
  screenMonitor->startMonitoring();
}

bool SystemMonitor::isOnBattery() { return batteryWatcher->isOnBattery; }
