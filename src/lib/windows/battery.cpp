// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"

#include <qglobal.h>
#include <windows.h>

#ifdef Q_OS_WIN

bool WindowsBatteryStatus::systemOnBattery() {
  SYSTEM_POWER_STATUS powerStatus;
  if (GetSystemPowerStatus(&powerStatus)) {
    return (powerStatus.ACLineStatus != 1);
  }
  return false;
}
#endif
