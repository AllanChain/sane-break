// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDBusConnection>
#include <QString>

namespace LinuxLogind {
// The object path of the logind session this process belongs to. Shared by the screen
// locker and the lock watcher so both agree on which session is "ours".
QString sessionPath(const QDBusConnection& bus);
}  // namespace LinuxLogind
