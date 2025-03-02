// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#include <QDBusInterface>

#ifdef Q_OS_LINUX
#ifndef SANE_BATTERY_LINUX_H
#define SANE_BATTERY_LINUX_H

#include "battery-status.h"  // IWYU pragma: export

class LinuxBatteryStatus : public BatteryStatus {
  Q_OBJECT
 public:
  LinuxBatteryStatus();
  bool systemOnBattery();

 private:
  QDBusInterface *iface;
};

#endif  // SANE_BATTERY_LINUX_H
#endif  // Q_OS_LINUX
