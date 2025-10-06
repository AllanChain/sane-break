// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// Private Internet Access - Desktop (https://github.com/pia-foss/desktop)
// Copyright (c) 2024 Private Internet Access, Inc.
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QWindow>

// Put a window on all workspaces on OS X (enables
// NSWindowCollectionBehaviorCanJoinAllSpaces)
void macSetAllWorkspaces(QWindow* window);
