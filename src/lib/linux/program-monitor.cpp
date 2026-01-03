
// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "program-monitor.h"

#include <QProcess>
#include <QString>
#include <QStringList>
#include <Qt>

const QStringList RunningProgramsMonitor::runningPrograms() {
  QProcess process;
  process.start("ps", QStringList() << "-e" << "-o" << "args=");
  process.waitForFinished();
  QString output = process.readAllStandardOutput();
  if (output.isEmpty()) return {};
  return output.split('\n', Qt::SkipEmptyParts);
}
