// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// Private Internet Access - Desktop (https://github.com/pia-foss/desktop)
// Copyright (c) 2024 Private Internet Access, Inc.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_MACOS
#import <AppKit/AppKit.h>
#include <QWindow>
#include "workspace.h"

namespace {
NSWindow *macGetNativeWindow(const QWindow *window) {
  // Casting from int to NSView * under ARC requires two steps - a cast
  // from int to *, then a bridge cast from a non-retainable pointer to a
  // retaininable pointer.
  void *pNativeViewVoid = reinterpret_cast<void *>(window->winId());
  // We don't hang on to the NSView *, so we can just bridge this cast
  // without retaining or releasing any references.
  // (The NSWindow * that we end up returning is handled normally by ARC,
  // the caller could hang on to it if they wanted to.)
  NSView *pNativeView = (__bridge NSView *)(pNativeViewVoid);
  return [pNativeView window];
}
}  // namespace

void macSetAllWorkspaces(QWindow *window) {
  NSWindow *pNativeWindow = macGetNativeWindow(window);
  [pNativeWindow setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces];
}
#endif  // Q_OS_MACOS
