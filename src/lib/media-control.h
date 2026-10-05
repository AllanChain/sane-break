// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// Best-effort pause of every media player that is currently active on the system.
//
// There is no cross-platform API for controlling other apps' playback, so each
// platform uses its native mechanism:
//   - Linux:   MPRIS2 over D-Bus
//   - Windows: GlobalSystemMediaTransportControlsSessionManager (WinRT)
//   - macOS:   private MediaRemote.framework (best effort; denied without an
//              entitlement since macOS 15.4)
//
// Implementations must never throw and must be safe to call from the GUI thread.
void pauseAllMedia();
