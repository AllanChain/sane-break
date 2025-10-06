// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "x11-idle.h"

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>

#include <cstddef>

int IdleTimeX11::read() {
  Display* dpy = XOpenDisplay(NULL);
  XScreenSaverInfo* ssi = XScreenSaverAllocInfo();
  XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), ssi);
  int idleTime = ssi->idle;
  XFree(ssi);
  XCloseDisplay(dpy);
  return idleTime;
}
