// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <windows.h>

// Detects whether an app is keeping the display awake on Windows.
//
// GetLastInputInfo (read by windowsIdleTime()) only tracks input, so a video player
// holding a display power request does not pause it. In InhibitorAware mode we detect
// such requests via CallNtPowerInformation(SystemExecutionState) — a documented,
// no-admin, no-spawn syscall that returns ES_DISPLAY_REQUIRED for *both* the legacy
// SetThreadExecutionState and the modern PowerSetRequest(PowerRequestDisplayRequired)
// paths (verified cross-process, Win 11 24H2 non-elevated). The admin-only
// powercfg/GetPowerRequestList path is deliberately *not* used.
//
// Cheap (single syscall), so it is polled on the idle tick directly via this free
// function rather than a cached monitor object.
bool windowsDisplayInhibited();
