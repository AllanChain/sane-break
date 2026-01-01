// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

#include <QTimer>

int macosIdleTime() {
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
