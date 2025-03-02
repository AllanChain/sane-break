// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <QList>
#include <QString>

namespace LinuxSystemSupport {
extern bool layerShell;
extern bool idleNotify;
extern bool idleGNOME;
extern bool trayIcon;
void check();
const QList<QString> errors();
const QList<QString> warnings();
}  // namespace LinuxSystemSupport

#endif
