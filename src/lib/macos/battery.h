// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_MACOS
#ifndef SANE_BATTERY_MACOS_H
#define SANE_BATTERY_MACOS_H

#include "battery-status.h"

class DarwinBatteryStatus : public BatteryStatus {
  Q_OBJECT
 public:
  bool systemOnBattery();
};

#endif  // SANE_BATTERY_MACOS_H
#endif  // Q_OS_MACOS
