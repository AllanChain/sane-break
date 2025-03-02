// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery-status.h"

#include <qglobal.h>

#include <QObject>
#include <QTimer>

#ifdef Q_OS_LINUX
#include "linux/battery.h"
#elif defined Q_OS_WIN
#include "windows/battery.h"
#elif defined Q_OS_MACOS
#include "macos/battery.h"
#endif

BatteryStatus* BatteryStatus::createWatcher() {
#ifdef Q_OS_LINUX
  return new LinuxBatteryStatus();
#elif defined Q_OS_WIN
  return new WindowsBatteryStatus();
#elif defined Q_OS_MACOS
  return new DarwinBatteryStatus();
#endif
}

BatteryStatus::BatteryStatus() : QObject() {
  timer = new QTimer();
  connect(timer, &QTimer::timeout, this, &BatteryStatus::tick);
}

void BatteryStatus::startWatching() {
  timer->setInterval(watchAccuracy);
  timer->start();
  tick();
}

void BatteryStatus::stopWatching() { timer->stop(); }

void BatteryStatus::tick() {
  bool currentOnBattery = systemOnBattery();
  if (currentOnBattery == isOnBattery) return;
  isOnBattery = currentOnBattery;
  if (isOnBattery)
    emit onBattery();
  else
    emit onPower();
}
