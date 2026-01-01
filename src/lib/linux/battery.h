// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusInterface>
#include <QObject>

#pragma once

#include "battery-status.h"  // IWYU pragma: export

class LinuxBatteryStatus : public BatteryStatus {
  Q_OBJECT
 public:
  LinuxBatteryStatus(QObject* parent = nullptr);
  bool systemOnBattery();

 private:
  QDBusInterface* iface;
};
