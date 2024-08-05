// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "idle-time.h"

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <QGuiApplication>

#include "linux/wayland/idle.h"
#include "linux/x11/idle.h"
#elif defined Q_OS_MACOS
#include "macos/idle.h"
#elif defined Q_OS_WIN
#include "windows/idle.h"
#endif

SystemIdleTime* SystemIdleTime::createIdleTimer() {
#ifdef Q_OS_LINUX
  if (QGuiApplication::platformName() == "wayland")
    return new IdleTimeWayland();
  else
    return new IdleTimeX11();
#elif defined Q_OS_MACOS
  return new IdleTimeDarwin();
#elif defined Q_OS_WIN
  return new IdleTimeWindows();
#endif
}

ReadBasedIdleTime::ReadBasedIdleTime() : SystemIdleTime() {
  timer = new QTimer();
  connect(timer, &QTimer::timeout, this, &ReadBasedIdleTime::tick);
}

void ReadBasedIdleTime::startWatching(WatchOption option) {
  switch (option) {
    case NOTIFY_FIRST_IDLE:
      isIdle = false;
      break;
    case NOTIFY_FIRST_RESUME:
      isIdle = true;
      break;
  }
  timer->setInterval(watchAccuracy);
  timer->start();
  tick();
}

void ReadBasedIdleTime::stopWatching() { timer->stop(); }

void ReadBasedIdleTime::tick() {
  int currentIdleTime = systemIdleTime();
  if (currentIdleTime < minIdleTime && isIdle) {
    isIdle = false;
    emit idleEnd(idleTime);
  } else if (currentIdleTime > minIdleTime && !isIdle) {
    isIdle = true;
    emit idleStart();
  }
  idleTime = currentIdleTime;
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
