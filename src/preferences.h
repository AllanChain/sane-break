// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_H
#define SANE_PREFERENCES_H

#include <QObject>
#include <QSettings>

class SanePreferences : public QObject {
  Q_OBJECT
 public:
  static int smallEvery();
  static int smallFor();
  static int bigAfter();
  static int bigFor();
  static int pauseOnIdleFor();
  static int resetOnIdleFor();
  static bool pauseOnBattery();
};

#endif  // SANE_PREFERENCES_H
