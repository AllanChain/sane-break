// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_FLAGS_H
#define SANE_FLAGS_H
#include <QFlags>

namespace SaneBreak {
enum class PauseReason {
  Idle = 1 << 0,
  OnBattery = 1 << 1,
  AppOpen = 1 << 2,
};
typedef QFlags<PauseReason> PauseReasons;
enum class BreakType { Small, Big };
};  // namespace SaneBreak
#endif  // !SANE_FLAGS_H
