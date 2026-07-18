// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "windows-inhibitor.h"

#include <powerbase.h>

#include <QDebug>

// ES_DISPLAY_REQUIRED: an app is requesting the display stay on.
#ifndef ES_DISPLAY_REQUIRED
#define ES_DISPLAY_REQUIRED 0x00000002
#endif

bool windowsDisplayInhibited() {
  // SystemExecutionState (level 16) returns a ULONG of execution-state flags. It
  // reflects cross-process SetThreadExecutionState/PowerSetRequest holds; a single
  // documented, no-admin syscall.
  ULONG execState = 0;
  NTSTATUS status = CallNtPowerInformation(SystemExecutionState, nullptr, 0, &execState,
                                           sizeof(execState));
  if (status != 0) return false;  // treat errors as "not inhibited"
  bool inhibited = (execState & ES_DISPLAY_REQUIRED) != 0;
  static bool lastInhibited = false;  // edge-triggered log; stateless free fn
  if (inhibited != lastInhibited) {
    lastInhibited = inhibited;
    qDebug() << "idle: windows inhibitor" << (inhibited ? "active" : "released");
  }
  return inhibited;
}
