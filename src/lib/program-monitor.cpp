// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "program-monitor.h"

#include <qglobal.h>

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <Qt>
#include <QtContainerFwd>
#include <utility>

RunningProgramsMonitor::RunningProgramsMonitor(QObject* parent)
    : QObject(parent), monitorTimer(new QTimer(this)) {
  connect(monitorTimer, &QTimer::timeout, this, &RunningProgramsMonitor::tick);
}

void RunningProgramsMonitor::startMonitoring() { monitorTimer->start(5000); }
void RunningProgramsMonitor::stopMonitoring() { monitorTimer->stop(); }

void RunningProgramsMonitor::setPrograms(const QStringList& programs) {
  programsToMonitor = programs.filter(validProgramFilter);
  if (programsToMonitor.isEmpty() && previouslySeen) {
    // Clean up: if all programs are removed from the list, stop pausing
    emit programStopped();
  }
  tick();
}

void RunningProgramsMonitor::tick() {
  if (programsToMonitor.isEmpty()) return;

  bool currentlySeen = false;
  for (const QString& program : runningPrograms()) {
    for (const QString& entry : std::as_const(programsToMonitor)) {
      if (program.contains(entry)) {
        currentlySeen = true;
        break;
      }
    }
  }
  if (!previouslySeen && currentlySeen) emit programStarted();
  if (previouslySeen && !currentlySeen) emit programStopped();
  previouslySeen = currentlySeen;
}
