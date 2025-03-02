// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
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
