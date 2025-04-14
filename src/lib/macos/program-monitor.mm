// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "program-monitor.h"

#include <AppKit/AppKit.h>

#include <QProcess>
#include <QString>
#include <QStringList>
#include <Qt>

const QStringList RunningProgramsMonitor::runningPrograms() {
  NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
  QStringList apps;
  for (NSRunningApplication *app in [sharedWorkspace runningApplications]) {
    apps.append(QString::fromNSString([[app executableURL] absoluteString]));
  }
  return apps;
}
