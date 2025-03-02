// Sane Break is a polite and sane break reminder preventing mindless skips.
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

int IdleTimeDarwin::systemIdleTime() {
  int idlesecs = -1;
  io_iterator_t iter = 0;
  if (IOServiceGetMatchingServices(kIOMainPortDefault, IOServiceMatching("IOHIDSystem"),
                                   &iter) == KERN_SUCCESS) {
    io_registry_entry_t entry = IOIteratorNext(iter);
    if (entry) {
      CFMutableDictionaryRef dict = NULL;
      if (IORegistryEntryCreateCFProperties(entry, &dict, kCFAllocatorDefault, 0) ==
          KERN_SUCCESS) {
        CFNumberRef obj = (CFNumberRef)CFDictionaryGetValue(dict, CFSTR("HIDIdleTime"));
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
