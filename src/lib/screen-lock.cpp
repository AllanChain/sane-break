// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#include <QProcess>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

bool lockScreen() {
#ifdef Q_OS_WIN
  return LockWorkStation();
#else
  QProcess process;

#ifdef Q_OS_MAC
  process.start("osascript", {"-e",
                              "tell application \"System Events\" to keystroke "
                              "\"q\" using {control down, command down}"});
#elif defined Q_OS_LINUX
  process.start("xdg-screensaver", {"lock"});
#endif

  process.waitForFinished();
  return process.exitCode() == 0;
#endif
}
