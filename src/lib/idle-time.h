// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>

#include "core/idle-time.h"

class ReadBasedIdleTime : public SystemIdleTime {
  Q_OBJECT
 public:
  ReadBasedIdleTime(QObject *parent);
  void startWatching();
  void stopWatching();
  virtual int systemIdleTime() { return 0; };
  void setWatchAccuracy(int accuracy);
  void setMinIdleTime(int idleTime);

 private:
  QTimer *timer;
  QTimer *delay;
  void tick();
};
