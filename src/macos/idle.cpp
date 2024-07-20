// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code adapted from
// Telegram Swift (https://github.com/overtake/TelegramSwift)
// Copyright © 2016 Telegram. All rights reserved.
// SPDX-License-Identifier: GPL-2.0-or-later

#include <qglobal.h>

#ifdef Q_OS_MACOS
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

#include <QTimer>

#include "idle.h"

IdleTimeDarwin::IdleTimeDarwin() : SystemIdleTime() {
  timer = new QTimer();
  connect(timer, &QTimer::timeout, this, &IdleTimeDarwin::tick);
}

void IdleTimeDarwin::startWatching(WatchOption option) {
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

void IdleTimeDarwin::stopWatching() { timer->stop(); }

void IdleTimeDarwin::tick() {
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
            // Divide by 10^9 to convert from nanoseconds to ms
            idlesecs = (nanoseconds >> 20);
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
#endif  // Q_OS_MACOS
