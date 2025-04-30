// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "idle-time.h"

#include <qglobal.h>

#include <QDateTime>
#include <QTimer>

#include "config.h"
#include "core/idle-time.h"

#ifdef Q_OS_LINUX
#include <QGuiApplication>
#include <QProcessEnvironment>

#ifdef ENABLE_X11
#include "linux/x11/idle.h"
#endif
#ifdef ENABLE_WAYLAND
#include <QDBusConnection>
#include <QDBusInterface>

#include "linux/wayland/idle.h"
#endif
#elif defined Q_OS_MACOS
#include "macos/idle.h"
#elif defined Q_OS_WIN
#include "windows/idle.h"
#endif

SystemIdleTime* SystemIdleTime::createIdleTimer(QObject* parent) {
#ifdef Q_OS_LINUX
  if (QGuiApplication::platformName() == "wayland") {
#ifdef ENABLE_WAYLAND
    QDBusInterface iface("org.gnome.Mutter.IdleMonitor",
                         "/org/gnome/Mutter/IdleMonitor/Core",
                         "org.gnome.Mutter.IdleMonitor", QDBusConnection::sessionBus());
    if (iface.isValid()) return new IdleTimeGNOME(parent);
    IdleTimeWayland* waylandIdleTimer = new IdleTimeWayland(parent);
    if (!waylandIdleTimer->isSupported())
      qFatal("Idle time detection is not supported");
    return waylandIdleTimer;
#else
    qFatal("Please compile with Wayland support.");
    return nullptr;
#endif
  } else {
#ifdef ENABLE_X11
    return new IdleTimeX11(parent);
#else
    qFatal("Please compile with X11 support.");
    return nullptr;
#endif
  }
#elif defined Q_OS_MACOS
  return new IdleTimeDarwin(parent);
#elif defined Q_OS_WIN
  return new IdleTimeWindows(parent);
#endif
}

ReadBasedIdleTime::ReadBasedIdleTime(QObject* parent) : SystemIdleTime(parent) {
  timer = new QTimer(this);
  delay = new QTimer(this);
  delay->setSingleShot(true);
  connect(timer, &QTimer::timeout, this, &ReadBasedIdleTime::tick);
  connect(delay, &QTimer::timeout, this, [this]() { timer->start(); });
}

void ReadBasedIdleTime::startWatching() {
  // Assume currently not idle and nofity first idle event
  m_isIdle = false;
  timer->setInterval(m_watchAccuracy);
  // We emit idleStart after at least m_minIdleTime msec.
  // In other words, we don't count idle times before watch starts.
  // This will be consistent with Wayland approaches.
  delay->setInterval(m_minIdleTime);
  delay->start();
}

void ReadBasedIdleTime::stopWatching() {
  delay->stop();
  timer->stop();
}

void ReadBasedIdleTime::tick() {
  int currentIdleTime = systemIdleTime();
  if (currentIdleTime < m_minIdleTime && m_isIdle) {
    m_isIdle = false;
    emit idleEnd();
  } else if (currentIdleTime > m_minIdleTime && !m_isIdle) {
    m_isIdle = true;
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
