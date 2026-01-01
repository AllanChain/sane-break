// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>
#include <functional>

#include "core/idle-time.h"
#include "idle-interface.h"

class ReadBasedIdleTime : public SystemIdleTime {
  Q_OBJECT
 public:
  ReadBasedIdleTime(QObject* parent, std::function<int()> idleReader);
  void startWatching() override;
  void stopWatching() override;
  void setWatchAccuracy(int accuracy) override;
  void setMinIdleTime(int idleTime) override;

 private:
  QTimer* m_timer;
  QTimer* m_delay;
  std::function<int()> m_idleReader = nullptr;
  void tick();
};
