// Sane Break is a polite break reminder to keep you sane.
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

int systemIdleTime();

class IdleTimeWindows : public SystemIdleTime {
  Q_OBJECT
 public:
  IdleTimeWindows();
  void startWatching(WatchOption option);
  void stopWatching();

 private:
  QTimer* timer;
  void tick();
  bool isIdle;
  int idleTime;
};

#endif  // SANE_IDLE_WINDOWS_H
#endif  // Q_OS_WIN
