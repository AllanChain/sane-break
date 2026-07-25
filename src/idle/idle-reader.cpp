// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "idle-reader.h"

#include <functional>
#include <utility>

#include "core/idle-time.h"

IdleReader::IdleReader(std::function<int()> rawReader, std::function<bool()> inhibited)
    : m_rawReader(std::move(rawReader)), m_inhibited(std::move(inhibited)) {}

int IdleReader::read(IdleMode mode) {
  int raw = m_rawReader();
  if (raw < 0) return raw;                      // propagate error sentinels
  if (mode == IdleMode::InputOnly) return raw;  // raw passthrough, inhibitor ignored
  if (m_inhibited()) {
    // Advance the baseline so that, on release, counting starts from 0 (the user is
    // treated as having just become active). This prevents an instant idleStart the
    // moment a long video ends.
    m_baseline = raw;
    return 0;  // "reset the idle time to zero"
  }
  int reported = raw - m_baseline;  // fresh count since release
  if (reported < 0) {
    // Raw dropped below the baseline (e.g. input during inhibition that the reader
    // still observed). Reset and follow raw.
    m_baseline = 0;
    reported = raw;
  }
  return reported;
}
