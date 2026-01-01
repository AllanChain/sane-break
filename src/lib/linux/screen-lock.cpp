// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QProcess>

bool lockScreen() {
  QProcess process;
  process.start("xdg-screensaver", {"lock"});
  process.waitForFinished();
  return process.exitCode() == 0;
}
