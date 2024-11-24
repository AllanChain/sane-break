// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "idle-time.h"

#include <qglobal.h>

#include <QDateTime>

#include "config.h"

#ifdef Q_OS_LINUX
#include <QGuiApplication>

#ifdef ENABLE_X11
#include "linux/x11/idle.h"
#endif
#ifdef ENABLE_WAYLAND
#include "linux/wayland/idle.h"
#endif
#elif defined Q_OS_MACOS
#include "macos/idle.h"
#elif defined Q_OS_WIN
#include "windows/idle.h"
#endif

SystemIdleTime* SystemIdleTime::createIdleTimer() {
#ifdef Q_OS_LINUX
  if (QGuiApplication::platformName() == "wayland") {
#ifdef ENABLE_WAYLAND
    return new IdleTimeWayland();
#else
    qFatal() << "Please compile with Wayland support.";
    return nullptr;
#endif
  } else {
#ifdef ENABLE_X11
    return new IdleTimeX11();
#else
    qFatal() << "Please compile with X11 support.";
    return nullptr;
#endif
  }
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

void ReadBasedIdleTime::startWatching() {
  // Assume currently not idle and nofity first idle event
  isIdle = false;
  timer->setInterval(m_watchAccuracy);
  timer->start();
  tick();
}

void ReadBasedIdleTime::stopWatching() { timer->stop(); }

void ReadBasedIdleTime::tick() {
  int currentIdleTime = systemIdleTime();
  if (currentIdleTime < m_minIdleTime && isIdle) {
    isIdle = false;
    emit idleEnd();
  } else if (currentIdleTime > m_minIdleTime && !isIdle) {
    isIdle = true;
    emit idleStart();
  }
}

void ReadBasedIdleTime::setMinIdleTime(int idleTime) { m_minIdleTime = idleTime; }

void ReadBasedIdleTime::setWatchAccuracy(int accuracy) {
  m_watchAccuracy = accuracy;
  timer->setInterval(accuracy);
  if (timer->isActive()) timer->stop();
  timer->start();
}

SleepMonitor::SleepMonitor() {
  timer = new QTimer(this);
  timer->setInterval(watchAccuracy);
  connect(timer, &QTimer::timeout, this, &SleepMonitor::tick);
  timer->start();
  lastAwake = QDateTime::currentMSecsSinceEpoch();
}

void SleepMonitor::tick() {
  int currentTime = QDateTime::currentMSecsSinceEpoch();
  if (currentTime - lastAwake > 2 * watchAccuracy) {
    emit sleepEnd();
  }
  lastAwake = currentTime;
}
