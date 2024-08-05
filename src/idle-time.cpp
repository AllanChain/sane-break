// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "idle-time.h"

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include "linux/wayland/idle.h"
#elif defined Q_OS_MACOS
#include "macos/idle.h"
#elif defined Q_OS_WIN
#include "windows/idle.h"
#endif

SystemIdleTime* SystemIdleTime::createIdleTimer() {
#ifdef Q_OS_LINUX
  return new IdleTimeWayland();
#elif defined Q_OS_MACOS
  return new IdleTimeDarwin();
#elif defined Q_OS_WIN
  return new IdleTimeWindows();
#endif
}

SleepMonitor::SleepMonitor() {
  timer = new QTimer(this);
  timer->setInterval(watchAccuracy);
  connect(timer, &QTimer::timeout, this, &SleepMonitor::tick);
  elapsedTimer = new QElapsedTimer();
  elapsedTimer->start();
}

void SleepMonitor::tick() {
  int elapsedTime = elapsedTimer->elapsed();
  if (elapsedTime > 2 * watchAccuracy) {
    emit sleepEnd(elapsedTime);
  }
  elapsedTimer->restart();
}
