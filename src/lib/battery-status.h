// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_BATTERY_H
#define SANE_BREAK_BATTERY_H

#include <QObject>
#include <QTimer>

class BatteryStatus : public QObject {
  Q_OBJECT
 public:
  BatteryStatus(QObject *parent = nullptr);
  void startWatching();
  void stopWatching();
  virtual bool systemOnBattery() { return true; };
  int watchAccuracy = 5000;  // How often we watch idle time (ms)
  static BatteryStatus *createWatcher(QObject *parent = nullptr);
  bool isOnBattery = false;

 private:
  QTimer *timer;
  void tick();

 signals:
  void onBattery();
  void onPower();
};

#endif  // SANE_BREAK_BATTERY_H
