#include "idle-time.h"

#include <qglobal.h>

#ifdef Q_OS_LINUX
#include "linux/kde/idle.h"
#elif defined Q_OS_MACOS
#include "macos/idle.h"
#elif defined Q_OS_WIN
#include "windows/idle.h"
#endif

SystemIdleTime* SystemIdleTime::createIdleTimer() {
#ifdef Q_OS_LINUX
  return new IdleTimeKDE();
#elif defined Q_OS_MACOS
  return new IdleTimeDarwin();
#elif defined Q_OS_WIN
  return new IdleTimeWindows();
#endif
}
