#include "system.h"

#include <qglobal.h>

#include "darwin.h"
#include "kde.h"
#include "windows.h"

SystemIdleTime* SystemIdleTime::createIdleTimer() {
#ifdef __linux
  return new IdleTimeKDE();
#elif defined __APPLE__
  return new IdleTimeDarwin();
#elif defined Q_OS_WIN
  return new IdleTimeWindows();
#endif
}
