// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// node-system-idle-time (https://github.com/anaisbetts/node-system-idle-time)
// Copyright (c) 2014 Atlassian Pty Ltd.
// SPDX-License-Identifier: Apache-2.0

#include <qglobal.h>

#ifdef Q_OS_WIN
#ifndef SANE_IDLE_WINDOWS_H
#define SANE_IDLE_WINDOWS_H

#include <QTimer>

#include "idle-time.h"

class IdleTimeWindows : public ReadBasedIdleTime {
  Q_OBJECT
 public:
  int systemIdleTime();
};

#endif  // SANE_IDLE_WINDOWS_H
#endif  // Q_OS_WIN
