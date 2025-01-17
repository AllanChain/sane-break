// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
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
