// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "layer-shell.h"

#include <LayerShellQt/shell.h>
#include <LayerShellQt/window.h>

#include <QWindow>

void LayerShell::init() { LayerShellQt::Shell::useLayerShell(); }

void LayerShell::layout(QWindow* window) {
  if (auto w = LayerShellQt::Window::get(window)) {
    using namespace LayerShellQt;
    w->setCloseOnDismissed(true);
    w->setLayer(Window::LayerOverlay);
    w->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
    w->setAnchors(Window::AnchorTop);
    // We do not want to reserve space for widgets like taskbar (#19)
    w->setExclusiveZone(-1);
  }
}
