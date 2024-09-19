// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_BATTERY_H
#define SANE_BREAK_BATTERY_H

#include <QObject>
#include <QTimer>

class BatteryStatus : public QObject {
  Q_OBJECT
 public:
  BatteryStatus();
  void startWatching();
  void stopWatching();
  virtual bool systemOnBattery() { return true; };
  int watchAccuracy = 5000;  // How often we watch idle time (ms)
  static BatteryStatus *createWatcher();

 private:
  QTimer *timer;
  void tick();
  bool isOnBattery = false;

 signals:
  void onBattery();
  void onPower();
};

#endif  // SANE_BREAK_BATTERY_H
