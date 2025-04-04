// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariant>

LinuxBatteryStatus::LinuxBatteryStatus(QObject* parent) : BatteryStatus(parent) {
  iface = new QDBusInterface(
      "org.freedesktop.UPower", "/org/freedesktop/UPower/devices/battery_BAT0",
      "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
}

bool LinuxBatteryStatus::systemOnBattery() {
  QDBusReply<QVariant> reply =
      iface->call("Get", "org.freedesktop.UPower.Device", "State");

  if (reply.isValid()) {
    int powerSupply = reply.value().toInt();
    return (powerSupply == 2);
  }
  return false;
}
