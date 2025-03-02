// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_WIN
#ifndef SANE_BATTERY_WIN_H
#define SANE_BATTERY_WIN_H

#include "battery-status.h"

class WindowsBatteryStatus : public BatteryStatus {
  Q_OBJECT
 public:
  bool systemOnBattery();
};

#endif  // SANE_BATTERY_WIN_H
#endif  // Q_OS_WIN
