// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "screen-lock-status.h"  // IWYU pragma: export

// Lock state comes from CGSessionCopyCurrentDictionary(); loginwindow's distributed
// com.apple.screenIsLocked / com.apple.screenIsUnlocked notifications only nudge a
// re-read. They are undocumented and can arrive without their counterpart, so trusting
// them as state would strand a paused schedule instead of costing a poll interval.
class DarwinScreenLockStatus : public ScreenLockStatus {
 public:
  DarwinScreenLockStatus(QObject* parent = nullptr);
  ~DarwinScreenLockStatus() override;
  bool systemScreenLocked() override;
};
