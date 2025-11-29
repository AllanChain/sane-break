// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <QFlags>

enum class PauseReason {
  Idle = 1 << 0,
  OnBattery = 1 << 1,
  AppOpen = 1 << 2,
  Sleep = 1 << 3,
  PostponeWindow = 1 << 4,
};
Q_DECLARE_FLAGS(PauseReasons, PauseReason)
Q_DECLARE_OPERATORS_FOR_FLAGS(PauseReasons)

enum class BreakType { Small, Big };

// The value of the enum is saved to db and should not change
enum class PostponeMode {
  BigBreaksOnly = 1,
  AllBreaks = 2,
  DisableForceBreaks = 10,
  Meeting = 20,
};
