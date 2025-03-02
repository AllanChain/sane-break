// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "program-monitor.h"

#include <qglobal.h>

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <Qt>
#include <QtContainerFwd>
#include <utility>

RunningProgramsMonitor::RunningProgramsMonitor(QObject *parent)
    : QObject(parent), monitorTimer(new QTimer(this)) {
  connect(monitorTimer, &QTimer::timeout, this, &RunningProgramsMonitor::tick);
}

void RunningProgramsMonitor::startMonitoring() { monitorTimer->start(5000); }
void RunningProgramsMonitor::stopMonitoring() { monitorTimer->stop(); }

void RunningProgramsMonitor::setPrograms(const QStringList &programs) {
  programsToMonitor = programs.filter(validProgramFilter);
  if (programsToMonitor.isEmpty() && previouslySeen) {
    // Clean up: if all programs are removed from the list, stop pausing
    emit programStopped();
  }
  tick();
}

void RunningProgramsMonitor::tick() {
  if (programsToMonitor.isEmpty()) return;
  QStringList runningPrograms;

  QProcess process;
#ifdef Q_OS_LINUX
  process.start("ps", QStringList() << "-e" << "-o" << "comm=");
#elif defined Q_OS_MACOS
  process.start("ps", QStringList() << "-ax" << "-o" << "comm=");
#elif defined Q_OS_WIN
  process.start("tasklist", QStringList() << "/fo" << "csv" << "/nh");
#endif

  process.waitForFinished();
  QString output = process.readAllStandardOutput();
  if (output.isEmpty()) return;

#ifdef Q_OS_WIN
  QStringList lines = output.split('\n', Qt::SkipEmptyParts);
  for (const QString &line : lines) {
    QStringList parts = line.split(',', Qt::SkipEmptyParts);
    if (parts.size() > 0) {
      QString program = parts[0].trimmed();
      runningPrograms.append(program);
    }
  }
#else
  runningPrograms = output.split('\n', Qt::SkipEmptyParts);
#endif

  bool currentlySeen = false;
  for (const QString &program : std::as_const(runningPrograms)) {
    for (const QString &entry : std::as_const(programsToMonitor)) {
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
