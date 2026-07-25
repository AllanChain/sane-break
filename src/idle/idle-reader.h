// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <functional>

#include "core/idle-time.h"

// Composes a raw idle source with an inhibitor flag and yields the effective idle time
// for a given IdleMode. This is the polling-backend implementation of the mode
// mechanism; the Wayland equivalent is the v1/v2 protocol-object switch.
//
//   read(InputOnly)      - raw input idle, passthrough (inhibitor ignored).
//   read(InhibitorAware) - idle held at 0 while inhibited; on release, counting
//                          starts from 0 via a captured baseline (matching Wayland
//                          v1 compositor semantics).
//
// Mode is passed to read() rather than stored, so it lives only on SystemIdleTime and
// there is no second copy to keep in sync. The baseline is inhibit-tracking state
// (not mode) and persists across reads here.
//
// Used by polling backends (GNOME, X11, macOS, Windows). Pure logic over injected
// callables, so it is unit-testable without a real platform.
class IdleReader {
 public:
  // rawReader  - returns raw input idle (ms), or a negative sentinel on error.
  // inhibited  - reads a cached bool: is an idle/display inhibitor active right now?
  IdleReader(std::function<int()> rawReader, std::function<bool()> inhibited);
  int read(IdleMode mode);

 private:
  std::function<int()> m_rawReader;
  std::function<bool()> m_inhibited;
  int m_baseline = 0;  // raw idle captured while inhibited
};
