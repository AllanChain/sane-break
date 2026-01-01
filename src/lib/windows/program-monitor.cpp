
// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "program-monitor.h"

#include <QProcess>
#include <QString>
#include <QStringList>
#include <Qt>

const QStringList RunningProgramsMonitor::runningPrograms() {
  QStringList programs;
  QProcess process;

  process.start("tasklist", QStringList() << "/fo" << "csv" << "/nh");
  process.waitForFinished();
  QString output = process.readAllStandardOutput();
  if (output.isEmpty()) return {};
  QStringList lines = output.split('\n', Qt::SkipEmptyParts);
  for (const QString& line : lines) {
    QStringList parts = line.split(',', Qt::SkipEmptyParts);
    if (parts.size() > 0) {
      QString program = parts[0].trimmed();
      programs.append(program);
    }
  }
  return programs;
}
