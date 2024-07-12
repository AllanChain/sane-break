// Copyright (c) 2024 Private Internet Access, Inc.
//
// This file is part of the Private Internet Access Desktop Client.
//
// The Private Internet Access Desktop Client is free software: you can
// redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// The Private Internet Access Desktop Client is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Private Internet Access Desktop Client.  If not, see
// <https://www.gnu.org/licenses/>.

#include "macos_window.h"
#import <AppKit/AppKit.h>
#include <QWindow>

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
