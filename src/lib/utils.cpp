// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "utils.h"

#include <QString>

QString formatTime(int seconds) {
  int minutes = seconds / 60;
  seconds %= 60;
  int hours = minutes / 60;
  minutes %= 60;
  if (hours == 0) {
    return QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
  } else {
    return QString("%1:%2:%3")
        .arg(hours)
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
  }
}
