// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <KIdleTime>

#include "idle.h"

IdleTimeKDE::IdleTimeKDE() : SystemIdleTime() {
  connect(KIdleTime::instance(),
          qOverload<int, int>(&KIdleTime::timeoutReached), this,
          [this](int id, int ms) {
            if (!isWatching || id != currentId) return;
            emit idleStart();
            KIdleTime::instance()->catchNextResumeEvent();
          });
  connect(KIdleTime::instance(), &KIdleTime::resumingFromIdle, this, [this]() {
    if (!isWatching) return;
    emit idleEnd();
    currentId = KIdleTime::instance()->addIdleTimeout(minIdleTime);
  });
}

void IdleTimeKDE::startWatching(WatchOption option) {
  isWatching = true;
  switch (option) {
    case NOTIFY_FIRST_IDLE:
      currentId = KIdleTime::instance()->addIdleTimeout(minIdleTime);
      break;
    case NOTIFY_FIRST_RESUME:
      KIdleTime::instance()->catchNextResumeEvent();
      break;
  }
}

void IdleTimeKDE::stopWatching() { isWatching = false; }
#endif  // Q_OS_LINUX
