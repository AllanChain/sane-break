// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
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
