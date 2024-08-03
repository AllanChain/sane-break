// Sane Break is a polite break reminder to keep you sane.
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
};

#endif  // SANE_PREFERENCES_H
