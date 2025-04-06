// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

#include "core/timer.h"

class Timer : public AbstractTimer {
  Q_OBJECT
 public:
  Timer(QObject *parent = nullptr) : AbstractTimer(parent) {
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &AbstractTimer::timeout);
  };
  ~Timer() = default;
  void start() override { timer->start(); };
  void start(int msec) override { timer->start(msec); };
  void stop() override { timer->stop(); };
  bool isActive() override { return timer->isActive(); };
  void setInterval(int msec) override { timer->setInterval(msec); };
  int interval() override { return timer->interval(); };
  void setSingleShot(bool singleShot) override { timer->setSingleShot(singleShot); };
  bool isSingleShot() override { return timer->isSingleShot(); };

 private:
  QTimer *timer;
};
