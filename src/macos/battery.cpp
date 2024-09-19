// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"

#include <qglobal.h>

#ifdef Q_OS_MACOS

bool DarwinBatteryStatus::systemOnBattery() { return false; }
#endif
