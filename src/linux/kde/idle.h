// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#ifndef SANE_IDLE_KDE_H
#define SANE_IDLE_KDE_H

#include <QTimer>

#include "idle-time.h"

int systemIdleTime();

class IdleTimeKDE : public SystemIdleTime {
  Q_OBJECT
 public:
  IdleTimeKDE();
  void startWatching(WatchOption option);
  void stopWatching();

 private:
  bool isWatching = false;
  int currentId = -1;
};

#endif  // SANE_IDLE_KDE_H
#endif  // Q_OS_LINUX
