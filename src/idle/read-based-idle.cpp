// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "read-based-idle.h"

#include <QObject>
#include <QTimer>
#include <functional>

#include "core/idle-time.h"

ReadBasedIdleTime::ReadBasedIdleTime(QObject *parent, std::function<int()> idleReader)
    : SystemIdleTime(parent), m_idleReader(idleReader) {
  m_timer = new QTimer(this);
  m_delay = new QTimer(this);
  m_delay->setSingleShot(true);
  connect(m_timer, &QTimer::timeout, this, &ReadBasedIdleTime::tick);
  connect(m_delay, &QTimer::timeout, this, [this]() { m_timer->start(); });
};

void ReadBasedIdleTime::startWatching() {
  // Assume currently not idle and nofity first idle event
  m_isIdle = false;
  m_timer->setInterval(m_watchAccuracy);
  // We emit idleStart after at least m_minIdleTime msec.
  // In other words, we don't count idle times before watch starts.
  // This will be consistent with Wayland approaches.
  m_delay->setInterval(m_minIdleTime);
  m_delay->start();
};

void ReadBasedIdleTime::stopWatching() {
  m_delay->stop();
  m_timer->stop();
};

void ReadBasedIdleTime::setWatchAccuracy(int accuracy) {
  m_watchAccuracy = accuracy;
  m_timer->setInterval(accuracy);
  if (m_timer->isActive()) m_timer->stop();
  m_timer->start();
};

void ReadBasedIdleTime::setMinIdleTime(int idleTime) { m_minIdleTime = idleTime; };

void ReadBasedIdleTime::tick() {
  int currentIdleTime = m_idleReader();
  if (currentIdleTime < m_minIdleTime && m_isIdle) {
    m_isIdle = false;
    emit idleEnd();
  } else if (currentIdleTime > m_minIdleTime && !m_isIdle) {
    m_isIdle = true;
    emit idleStart();
  }
};
