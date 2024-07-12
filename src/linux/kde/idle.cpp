#include <qglobal.h>

#ifdef Q_OS_LINUX
#include <KIdleTime>

#include "idle.h"

IdleTimeKDE::IdleTimeKDE() : SystemIdleTime() {
  connect(KIdleTime::instance(), &KIdleTime::timeoutReached, this, [this]() {
    emit idleStart();
    if (isWatching) KIdleTime::instance()->catchNextResumeEvent();
  });
  connect(KIdleTime::instance(), &KIdleTime::resumingFromIdle, this, [this]() {
    emit idleEnd();
    if (isWatching) KIdleTime::instance()->addIdleTimeout(2000);
  });
}

void IdleTimeKDE::startWatching() {
  isWatching = true;
  KIdleTime::instance()->catchNextResumeEvent();
}

void IdleTimeKDE::stopWatching() { isWatching = false; }
#endif  // Q_OS_LINUX
