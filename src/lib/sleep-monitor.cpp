// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lib/sleep-monitor.h"

#include <QDateTime>
#include <QObject>
#include <QTimer>

SleepMonitor::SleepMonitor(QObject* parent) : QObject(parent) {
  timer = new QTimer(this);
  timer->setInterval(watchAccuracy);
  connect(timer, &QTimer::timeout, this, &SleepMonitor::tick);
  timer->start();
  lastAwake = QDateTime::currentMSecsSinceEpoch();
}

void SleepMonitor::tick() {
  int currentTime = QDateTime::currentMSecsSinceEpoch();
  int elapsedTime = currentTime - lastAwake;
  lastAwake = currentTime;
  if (elapsedTime > 2 * watchAccuracy) {
    emit sleepEnd(elapsedTime / 1000);
  }
}
