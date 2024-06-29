#ifdef __APPLE__
#include "darwin.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

#include <QTimer>

IdleTimeDarwin::IdleTimeDarwin() : SystemIdleTime() {
  timer = new QTimer();
  timer->setInterval(500);
  isIdle = true;
  connect(timer, &QTimer::timeout, this, &IdleTimeDarwin::tick);
}

void IdleTimeDarwin::startWatching() {
  timer->start();
  tick();
}

void IdleTimeDarwin::stopWatching() { timer->stop(); }

void IdleTimeDarwin::tick() {
  int idleTime = systemIdleTime();
  if (idleTime == 0 && isIdle) {
    isIdle = false;
    emit idleEnd();
  } else if (idleTime > 0 && !isIdle) {
    isIdle = true;
    emit idleStart();
  }
}

int systemIdleTime() {
  int idlesecs = -1;
  io_iterator_t iter = 0;
  if (IOServiceGetMatchingServices(kIOMainPortDefault,
                                   IOServiceMatching("IOHIDSystem"),
                                   &iter) == KERN_SUCCESS) {
    io_registry_entry_t entry = IOIteratorNext(iter);
    if (entry) {
      CFMutableDictionaryRef dict = NULL;
      if (IORegistryEntryCreateCFProperties(entry, &dict, kCFAllocatorDefault,
                                            0) == KERN_SUCCESS) {
        CFNumberRef obj =
            (CFNumberRef)CFDictionaryGetValue(dict, CFSTR("HIDIdleTime"));
        if (obj) {
          int64_t nanoseconds = 0;
          if (CFNumberGetValue(obj, kCFNumberSInt64Type, &nanoseconds)) {
            // Divide by 10^9 to convert from nanoseconds to seconds.
            idlesecs = (nanoseconds >> 30);
          }
        }
        CFRelease(dict);
      }
      IOObjectRelease(entry);
    }
    IOObjectRelease(iter);
  }
  return idlesecs;
}
#endif  // __APPLE__
