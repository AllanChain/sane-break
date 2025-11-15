// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "app/windows/signal-handler.h"

#include <windows.h>

#include <QCoreApplication>

bool SignalHandler::nativeEventFilter(const QByteArray& eventType, void* message,
                                      qintptr* result) {
  if (eventType == "windows_generic_MSG") {
    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_QUERYENDSESSION) {
      *result = TRUE;
      return true;
    } else if (msg->message == WM_ENDSESSION) {
      if (msg->wParam) {
        QCoreApplication::quit();
        return true;
      }
    }
  }
  return false;
}
