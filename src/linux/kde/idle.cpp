// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <KIdleTime>

#include "idle.h"

IdleTimeKDE::IdleTimeKDE() : SystemIdleTime() {
  elapseTimer = new QElapsedTimer();
  connect(KIdleTime::instance(),
          qOverload<int, int>(&KIdleTime::timeoutReached), this,
          [this](int id, int ms) {
            if (!isWatching || id != currentId || isIdle) return;
            emit idleStart();
            isIdle = true;
            KIdleTime::instance()->catchNextResumeEvent();
            elapseTimer->start();
          });
  connect(KIdleTime::instance(), &KIdleTime::resumingFromIdle, this, [this]() {
    if (!isWatching || !isIdle) return;
    int elapsedTime = elapseTimer->elapsed();
    // If this is not the first resume event
    if (currentId != -1) elapsedTime += minIdleTime;
    emit idleEnd(elapsedTime);
    isIdle = false;
    currentId = KIdleTime::instance()->addIdleTimeout(minIdleTime);
  });
}

void IdleTimeKDE::startWatching(WatchOption option) {
  isWatching = true;
  elapseTimer->start();
  switch (option) {
    case NOTIFY_FIRST_IDLE:
      isIdle = false;
      currentId = KIdleTime::instance()->addIdleTimeout(minIdleTime);
      break;
    case NOTIFY_FIRST_RESUME:
      isIdle = true;
      KIdleTime::instance()->catchNextResumeEvent();
      break;
  }
}

void IdleTimeKDE::stopWatching() { isWatching = false; }
#endif  // Q_OS_LINUX
