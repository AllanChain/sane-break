// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "x11-idle.h"

// clang-format off
#include <QDebug>
#include <cstddef>

#include <X11/Xlib.h>
#include <X11/extensions/saver.h>
#include <X11/extensions/scrnsaver.h>
// clang-format on

int IdleTimeX11::read() {
  Display* dpy = XOpenDisplay(NULL);
  XScreenSaverInfo* ssi = XScreenSaverAllocInfo();
  XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), ssi);
  int idleTime = ssi->idle;
  // state == ScreenSaverDisabled means a client is holding a XScreenSaverSuspend(True)
  // inhibition — the standard programmatic X11 inhibition path used by most media
  // players. Best-effort: apps that inhibit via DPMS-direct or timer-resetting are
  // not detected (documented limitation).
  bool inhibited = (ssi->state == ScreenSaverDisabled);
  if (inhibited != m_inhibited)
    qDebug() << "idle: x11 inhibitor" << (inhibited ? "active" : "released");
  m_inhibited = inhibited;
  XFree(ssi);
  XCloseDisplay(dpy);
  return idleTime;
}
