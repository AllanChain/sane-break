// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariant>

LinuxBatteryStatus::LinuxBatteryStatus() : BatteryStatus() {
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
#endif
