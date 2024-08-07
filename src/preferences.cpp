// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "preferences.h"

int SanePreferences::smallEvery() {
  QSettings settings;
  return settings.value("break/small-every", 1200).toInt();
}
int SanePreferences::smallFor() {
  QSettings settings;
  return settings.value("break/small-for", 20).toInt();
}
int SanePreferences::bigAfter() {
  QSettings settings;
  return settings.value("break/big-after", 3).toInt();
}
int SanePreferences::bigFor() {
  QSettings settings;
  return settings.value("break/big-for", 60).toInt();
}
int SanePreferences::pauseOnIdleFor() {
  QSettings settings;
  return settings.value("break/pause-on-idle-for", 60).toInt();
}
int SanePreferences::resetOnIdleFor() {
  QSettings settings;
  return settings.value("break/reset-on-idle-for", 300).toInt();
}
