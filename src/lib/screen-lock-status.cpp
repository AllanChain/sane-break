// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "screen-lock-status.h"

#include <qglobal.h>

#include <QDebug>
#include <QObject>
#include <QTimer>

#ifdef Q_OS_LINUX
#include "linux/lock-status.h"
#elif defined Q_OS_WIN
#include "windows/lock-status.h"
#elif defined Q_OS_MACOS
#include "macos/lock-status.h"
#endif

ScreenLockStatus* ScreenLockStatus::createWatcher(QObject* parent) {
#ifdef Q_OS_LINUX
  return new LinuxScreenLockStatus(parent);
#elif defined Q_OS_WIN
  return new WindowsScreenLockStatus(parent);
#elif defined Q_OS_MACOS
  return new DarwinScreenLockStatus(parent);
#endif
}

ScreenLockStatus::ScreenLockStatus(QObject* parent) : QObject(parent) {
  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &ScreenLockStatus::check);
}

void ScreenLockStatus::startWatching() {
  timer->setInterval(watchAccuracy);
  timer->start();
  // Report the state we start in: launching on a locked session pauses the schedule
  // instead of counting down to a break nobody will see.
  check();
}

void ScreenLockStatus::stopWatching() { timer->stop(); }

void ScreenLockStatus::check() {
  bool currentlyLocked = systemScreenLocked();
  if (currentlyLocked == isLocked) return;
  isLocked = currentlyLocked;
  // Transitions only. No line here during a lock means the desktop never reported one.
  qDebug() << "pause:" << (isLocked ? "screen locked" : "screen unlocked");
  if (isLocked)
    emit screenLocked();
  else
    emit screenUnlocked();
}
