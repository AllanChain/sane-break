// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "inhibitor-proxy-idle.h"

#include <QObject>

#include "core/idle-time.h"
#include "linux-idle-inhibitor.h"

InhibitorProxyIdleTime::InhibitorProxyIdleTime(SystemIdleTime* source,
                                               LinuxIdleInhibitor* inhibitor,
                                               QObject* parent)
    : SystemIdleTime(parent), m_source(source), m_inhibitor(inhibitor) {
  m_source->setParent(this);
  m_inhibitor->setParent(this);
  connect(m_source, &SystemIdleTime::idleStart, this,
          &InhibitorProxyIdleTime::onSourceIdleStart);
  connect(m_source, &SystemIdleTime::idleEnd, this,
          &InhibitorProxyIdleTime::onSourceIdleEnd);
  connect(m_inhibitor, &LinuxIdleInhibitor::inhibitorChanged, this,
          &InhibitorProxyIdleTime::onInhibitorChanged);
}

void InhibitorProxyIdleTime::startWatching() {
  m_isIdle = false;
  m_source->startWatching();
}

void InhibitorProxyIdleTime::stopWatching() { m_source->stopWatching(); }

void InhibitorProxyIdleTime::setWatchAccuracy(int accuracy) {
  m_watchAccuracy = accuracy;
  m_source->setWatchAccuracy(accuracy);
}

void InhibitorProxyIdleTime::setMinIdleTime(int idleTime) {
  m_minIdleTime = idleTime;
  m_source->setMinIdleTime(idleTime);
}

void InhibitorProxyIdleTime::setIdleMode(IdleMode mode) {
  m_idleMode = mode;
  m_source->setIdleMode(mode);
  reevaluate();
}

void InhibitorProxyIdleTime::onSourceIdleStart() { reevaluate(); }

void InhibitorProxyIdleTime::onSourceIdleEnd() { reevaluate(); }

void InhibitorProxyIdleTime::onInhibitorChanged() { reevaluate(); }

void InhibitorProxyIdleTime::reevaluate() {
  bool shouldBeIdle = m_source->isIdle();
  if (m_idleMode == IdleMode::InhibitorAware && m_inhibitor->isIdleInhibited())
    shouldBeIdle = false;
  if (shouldBeIdle && !m_isIdle) {
    m_isIdle = true;
    emit idleStart();
  } else if (!shouldBeIdle && m_isIdle) {
    m_isIdle = false;
    emit idleEnd();
  }
}
