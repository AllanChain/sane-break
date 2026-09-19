// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "screen-lock-status.h"  // IWYU pragma: export

// Watches the session lock through WTS session change notifications, the documented way
// to learn about Win+L, with the inherited poll as the state query: a notification only
// says a transition was requested, and it can be missed (a locked workstation reached
// through fast user switching).
//
// The state is the session's lock flag from WTSSessionInfoEx. The desktop a window is
// on cannot answer it: a lock screen that nobody has touched is drawn on the
// interactive desktop, and only the credential prompt moves input to a secure one.
//
// Notifications are delivered to a window, so this owns a hidden message-only window
// for the duration of the watch. It is plain Win32, not a Qt window.
class WindowsScreenLockStatus : public ScreenLockStatus {
 public:
  WindowsScreenLockStatus(QObject* parent = nullptr);
  ~WindowsScreenLockStatus() override;
  bool systemScreenLocked() override;

  // Public only because the window procedure is a plain C callback; it nudges a read.
  void onSessionChange(unsigned int event);

 private:
  // HWND of the message-only window, as void* so this header stays <windows.h>-free.
  void* m_sessionEventsWindow = nullptr;
  // The state read can be refused for the whole run, so report it once, not per poll.
  bool m_loggedReadFailure = false;
};
