// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

class SleepMonitor : public QObject {
  Q_OBJECT
 public:
  SleepMonitor(QObject* parent = nullptr);
  int watchAccuracy = 5000;  // in ms
 signals:
  void sleepEnd(int sleptSeconds);

 private:
  QTimer* timer;
  int lastAwake;
  void tick();
};
