// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "battery-status.h"

class DarwinBatteryStatus : public BatteryStatus {
  Q_OBJECT
 public:
  using BatteryStatus::BatteryStatus;
  bool systemOnBattery();
};
