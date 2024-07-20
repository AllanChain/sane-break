// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// node-system-idle-time (https://github.com/anaisbetts/node-system-idle-time)
// Copyright (c) 2014 Atlassian Pty Ltd.
// SPDX-License-Identifier: Apache-2.0

#include <qglobal.h>

#ifdef Q_OS_WIN

#include <windows.h>

#include <QTimer>

#include "idle.h"

IdleTimeWindows::IdleTimeWindows() : SystemIdleTime() {
  timer = new QTimer();
  connect(timer, &QTimer::timeout, this, &IdleTimeWindows::tick);
}

void IdleTimeWindows::startWatching(WatchOption option) {
  switch (option) {
    case NOTIFY_FIRST_IDLE:
      isIdle = false;
      break;
    case NOTIFY_FIRST_RESUME:
      isIdle = true;
      break;
  }
  timer->setInterval(watchAccuracy);
  timer->start();
  tick();
}

void IdleTimeWindows::stopWatching() { timer->stop(); }

void IdleTimeWindows::tick() {
  int idleTime = systemIdleTime();
  if (idleTime < minIdleTime && isIdle) {
    isIdle = false;
    emit idleEnd();
  } else if (idleTime > minIdleTime && !isIdle) {
    isIdle = true;
    emit idleStart();
  }
}

int systemIdleTime() {
  LASTINPUTINFO lif;
  lif.cbSize = sizeof(lif);
  if (!GetLastInputInfo(&lif)) return -1;
  uint64_t tickCount = GetTickCount64();
  int idleTime = (tickCount - (uint64_t)lif.dwTime);
  return idleTime;
}
#endif  // Q_OS_WIN
