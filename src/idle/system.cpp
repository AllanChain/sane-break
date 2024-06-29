#include "system.h"

#include "darwin.h"
#include "kde.h"

SystemIdleTime* SystemIdleTime::createIdleTimer() {
#ifdef __linux
  return new IdleTimeKDE();
#elif __APPLE__
#include "darwin.h"
  return new IdleTimeDarwin();
#endif
}
