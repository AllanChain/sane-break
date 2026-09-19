// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lock-status.h"

#include <windows.h>
#include <wtsapi32.h>

#include <QDebug>

namespace {
constexpr wchar_t kWindowClassName[] = L"SaneBreak.SessionEvents";

HMODULE moduleHandle() { return GetModuleHandleW(nullptr); }

LRESULT CALLBACK sessionEventsWindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                         LPARAM lParam) {
  if (message == WM_WTSSESSION_CHANGE) {
    auto* self = static_cast<WindowsScreenLockStatus*>(
        reinterpret_cast<void*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
    if (self) self->onSessionChange(static_cast<unsigned int>(wParam));
    return 0;
  }
  return DefWindowProcW(hwnd, message, wParam, lParam);
}

// Registers the window class once per process. A second ScreenLockStatus would just
// create another window of the same class.
bool ensureWindowClassRegistered() {
  static const bool registered = [] {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = sessionEventsWindowProc;
    windowClass.hInstance = moduleHandle();
    windowClass.lpszClassName = kWindowClassName;
    return RegisterClassExW(&windowClass) != 0;
  }();
  return registered;
}

HWND createSessionEventsWindow() {
  if (!ensureWindowClassRegistered()) return nullptr;
  // A message-only window: it never appears on screen and is not part of the
  // alt+tab list, but it receives the posted session notifications like any other.
  return CreateWindowExW(0, kWindowClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                         moduleHandle(), nullptr);
}

// The session lock, from the terminal server, which tracks it independently of which
// desktop happens to be receiving input: a lock screen that nobody has touched is drawn
// on the interactive desktop, and only the credential prompt moves input to a secure
// one. The desktop a window is on is therefore not the lock state, but this is.
enum class SessionLock {
  Locked,
  Unlocked,
  Unknown,  // no answer at all, so nothing was learned
};

SessionLock readSessionLock(DWORD* error) {
  *error = 0;
  PWTSINFOEXW info = nullptr;
  DWORD bytes = 0;
  if (!WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION,
                                   WTSSessionInfoEx, reinterpret_cast<LPWSTR*>(&info),
                                   &bytes) ||
      !info) {
    *error = GetLastError();
    return SessionLock::Unknown;
  }
  SessionLock state = SessionLock::Unknown;
  if (info->Level == 1) {
    switch (info->Data.WTSInfoExLevel1.SessionFlags) {
      case WTS_SESSIONSTATE_LOCK:
        state = SessionLock::Locked;
        break;
      case WTS_SESSIONSTATE_UNLOCK:
        state = SessionLock::Unlocked;
        break;
      default:  // WTS_SESSIONSTATE_UNKNOWN
        break;
    }
  } else {
    *error = ERROR_INVALID_DATA;
  }
  WTSFreeMemory(info);
  return state;
}
}  // namespace

WindowsScreenLockStatus::WindowsScreenLockStatus(QObject* parent)
    : ScreenLockStatus(parent) {
  m_sessionEventsWindow = createSessionEventsWindow();
  if (!m_sessionEventsWindow) {
    qDebug() << "pause: no message window, polling screen lock state instead";
    return;
  }
  // Bound to the window, and dropped with it, so this client only ever hears about its
  // own session. The association is set after creation rather than in WM_NCCREATE
  // because WM_WTSSESSION_CHANGE is posted: nothing can reach the window procedure
  // before this call returns to the event loop.
  SetWindowLongPtrW(static_cast<HWND>(m_sessionEventsWindow), GWLP_USERDATA,
                    reinterpret_cast<LONG_PTR>(this));
  if (!WTSRegisterSessionNotification(static_cast<HWND>(m_sessionEventsWindow),
                                      NOTIFY_FOR_THIS_SESSION)) {
    qDebug() << "pause: WTSRegisterSessionNotification failed" << GetLastError()
             << "- polling screen lock state instead";
  }
}

WindowsScreenLockStatus::~WindowsScreenLockStatus() {
  if (!m_sessionEventsWindow) return;
  WTSUnRegisterSessionNotification(static_cast<HWND>(m_sessionEventsWindow));
  DestroyWindow(static_cast<HWND>(m_sessionEventsWindow));
}

void WindowsScreenLockStatus::onSessionChange(unsigned int event) {
  switch (event) {
    case WTS_SESSION_LOCK:
    case WTS_SESSION_UNLOCK:
    case WTS_CONSOLE_DISCONNECT:
    case WTS_CONSOLE_CONNECT:
      // Nudge only. Which of these arrives is not the lock state: a console disconnect
      // also happens when a remote session takes the display over, and the lock screen
      // can be reached without either event.
      check();
      break;
    default:
      break;  // logon/logoff and the reserved values concern other sessions
  }
}

bool WindowsScreenLockStatus::systemScreenLocked() {
  DWORD error = 0;
  const SessionLock state = readSessionLock(&error);
  switch (state) {
    case SessionLock::Locked:
    case SessionLock::Unlocked:
      m_loggedReadFailure = false;  // A later failure is worth reporting on its own
      return state == SessionLock::Locked;
    case SessionLock::Unknown:
      break;
  }
  // A session that cannot answer this keeps not answering it, and without this line a
  // machine where the feature cannot work looks like one that never locked.
  if (!m_loggedReadFailure) {
    m_loggedReadFailure = true;
    qDebug() << "pause: cannot read the session lock state" << error
             << ", screen lock pause disabled";
  }
  return false;  // An unconfirmed state must not look like a lock
}
