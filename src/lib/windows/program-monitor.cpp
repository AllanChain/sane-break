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

  // First attempt: Use PowerShell to get full command lines
  process.start("powershell",
                QStringList() << "-NoProfile"
                              << "-NonInteractive"
                              << "-Command"
                              << "Get-CimInstance Win32_Process | Select CommandLine");

  if (process.waitForFinished(5000)) {  // 5 second timeout
    QString output = process.readAllStandardOutput();

    if (!output.isEmpty() && process.exitCode() == 0) {
      QStringList lines = output.split('\n', Qt::SkipEmptyParts);
      for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.isEmpty()) {
          programs.append(trimmed);
        }
      }

      if (!programs.isEmpty()) {
        return programs;
      }
    }
  }

  // Fallback: Use tasklist if PowerShell failed
  programs.clear();
  process.close();

  process.start("tasklist", QStringList() << "/fo" << "csv" << "/nh");
  process.waitForFinished();

  QString output = process.readAllStandardOutput();
  if (output.isEmpty()) return {};

  QStringList lines = output.split('\n', Qt::SkipEmptyParts);
  for (const QString& line : lines) {
    QStringList parts = line.split(',', Qt::SkipEmptyParts);
    if (parts.size() > 0) {
      QString program = parts[0].trimmed();
      // Remove surrounding quotes if present
      if (program.startsWith('"') && program.endsWith('"')) {
        program = program.mid(1, program.length() - 2);
      }
      programs.append(program);
    }
  }

  return programs;
}
