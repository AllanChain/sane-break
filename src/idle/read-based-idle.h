// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QTimer>
#include <functional>
#include <memory>

#include "core/idle-time.h"
#include "idle-reader.h"

class ReadBasedIdleTime : public SystemIdleTime {
  Q_OBJECT
 public:
  ReadBasedIdleTime(QObject* parent, std::function<int()> idleReader);
  void startWatching() override;
  void stopWatching() override;
  void setWatchAccuracy(int accuracy) override;
  void setMinIdleTime(int idleTime) override;
  // Install an inhibitor monitor. When present, tick() routes the raw reader through
  // IdleReader, which zeroes idle while inhibited (in InhibitorAware mode) and passes
  // it through unchanged (in InputOnly mode). No-op if never called.
  void setInhibitor(std::function<bool()> inhibited);
  bool isInhibited() const override { return m_inhibitedFn ? m_inhibitedFn() : false; }

 private:
  QTimer* m_timer;
  QTimer* m_delay;
  std::function<int()> m_rawReader;     // raw input idle (always available)
  std::function<bool()> m_inhibitedFn;  // cached for isInhibited()
  // When set, read(m_idleMode) applies inhibitor-aware zeroing; otherwise raw is
  // used directly. Mode is read from the base SystemIdleTime::m_idleMode at tick.
  std::unique_ptr<IdleReader> m_reader;
  void tick();
};
