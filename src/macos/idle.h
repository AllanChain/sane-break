// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// Telegram Swift (https://github.com/overtake/TelegramSwift)
// Copyright © 2016 Telegram. All rights reserved.
// SPDX-License-Identifier: GPL-2.0-or-later

#include <qglobal.h>

#ifdef Q_OS_MACOS
#ifndef SANE_IDLE_DARWIN_H
#define SANE_IDLE_DARWIN_H

#include <QTimer>

#include "idle-time.h"

int systemIdleTime();

class IdleTimeDarwin : public ReadBasedIdleTime {
  Q_OBJECT
 public:
  int systemIdleTime();
};

#endif  // SANE_IDLE_DARWIN_H
#endif  // Q_OS_MACOS
