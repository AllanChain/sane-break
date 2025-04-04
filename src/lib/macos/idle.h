// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// Telegram Swift (https://github.com/overtake/TelegramSwift)
// Copyright © 2016 Telegram. All rights reserved.
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef SANE_IDLE_DARWIN_H
#define SANE_IDLE_DARWIN_H

#include <QTimer>

#include "idle-time.h"

int systemIdleTime();

class IdleTimeDarwin : public ReadBasedIdleTime {
  Q_OBJECT
 public:
  using ReadBasedIdleTime::ReadBasedIdleTime;
  int systemIdleTime();
};

#endif  // SANE_IDLE_DARWIN_H
