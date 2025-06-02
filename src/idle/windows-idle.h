// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <windows.h>

#include <QTimer>

int windowsIdleTime() {
  LASTINPUTINFO lif;
  lif.cbSize = sizeof(lif);
  if (!GetLastInputInfo(&lif)) return -1;
  uint64_t tickCount = GetTickCount64();
  int idleTime = (tickCount - (uint64_t)lif.dwTime);
  return idleTime;
}
