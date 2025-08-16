// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "core/flags.h"

class AbstractSystemMonitor : public QObject {
  Q_OBJECT
 public:
  AbstractSystemMonitor(QObject *parent = nullptr) : QObject(parent) {};
  ~AbstractSystemMonitor() = default;
  virtual void start() = 0;
  virtual bool isOnBattery() = 0;

 signals:
  void sleepEnded(int sleptSeconds);
  void pauseRequested(SaneBreak::PauseReason reason);
  void resumeRequested(SaneBreak::PauseReason reason);
};
