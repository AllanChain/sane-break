#ifdef __linux
#include "kde.h"

#include <KIdleTime>

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
#endif  // __linux
