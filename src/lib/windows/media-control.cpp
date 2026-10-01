// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "media-control.h"

// Keep this translation unit free of Qt headers: the C++/WinRT headers pull in
// <windows.h>, whose macros (min/max, etc.) would otherwise fight with Qt.
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>

#include <thread>

namespace {

void pauseAllMediaWorker() {
  // The thread starts uninitialized; C++/WinRT needs an apartment. Use MTA so the
  // blocking .get() calls below never marshal back to (and deadlock) the Qt STA
  // main thread.
  winrt::init_apartment(winrt::apartment_type::multi_threaded);
  try {
    using namespace winrt::Windows::Media::Control;
    auto manager =
        GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
    if (manager) {
      for (const auto& session : manager.GetSessions()) {
        try {
          session.TryPauseAsync().get();
        } catch (...) {
          // A session can disappear between enumeration and the call; ignore.
        }
      }
    }
  } catch (...) {
    // No session manager (unsupported OS build, or access denied) — nothing to do.
  }
  winrt::uninit_apartment();
}

}  // namespace

void pauseAllMedia() {
  // Detach: this is fire-and-forget, and the work must not block the GUI thread.
  std::thread(pauseAllMediaWorker).detach();
}
