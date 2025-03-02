// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// node-system-idle-time (https://github.com/anaisbetts/node-system-idle-time)
// Copyright (c) 2014 Atlassian Pty Ltd.
// SPDX-License-Identifier: Apache-2.0

#include "idle.h"

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>

int IdleTimeX11::systemIdleTime() {
  Display *dpy = XOpenDisplay(NULL);
  XScreenSaverInfo *ssi = XScreenSaverAllocInfo();
  XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), ssi);
  int idleTime = ssi->idle;
  XFree(ssi);
  XCloseDisplay(dpy);
  return idleTime;
}
