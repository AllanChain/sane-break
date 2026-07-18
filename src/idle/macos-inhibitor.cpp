// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "macos-inhibitor.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/pwr_mgt/IOPMLib.h>

#include <QDebug>
#include <QTimer>

MacosInhibitor::MacosInhibitor(QObject* parent) : QObject(parent) {
  m_timer = new QTimer(this);
  m_timer->setInterval(2000);  // slow poll; the per-tick reader only reads the cache
  connect(m_timer, &QTimer::timeout, this, &MacosInhibitor::refresh);
  refresh();
  m_timer->start();
}

void MacosInhibitor::refresh() {
  bool inhibited = false;
  // Returns a dictionary mapping PID (CFNumberRef) -> CFArrayRef of assertion dicts.
  CFDictionaryRef assertionsByProcess = nullptr;
  if (IOPMCopyAssertionsByProcess(&assertionsByProcess) == kIOReturnSuccess &&
      assertionsByProcess != nullptr) {
    CFIndex count = CFDictionaryGetCount(assertionsByProcess);
    if (count > 0) {
      const void** keys = new const void*[count];
      const void** values = new const void*[count];
      CFDictionaryGetKeysAndValues(assertionsByProcess, keys, values);
      for (CFIndex i = 0; i < count && !inhibited; ++i) {
        CFArrayRef assertions = static_cast<CFArrayRef>(values[i]);
        CFIndex n = CFArrayGetCount(assertions);
        for (CFIndex j = 0; j < n; ++j) {
          CFDictionaryRef dict =
              static_cast<CFDictionaryRef>(CFArrayGetValueAtIndex(assertions, j));
          CFStringRef type = static_cast<CFStringRef>(
              CFDictionaryGetValue(dict, kIOPMAssertionTypeKey));
          CFNumberRef level = static_cast<CFNumberRef>(
              CFDictionaryGetValue(dict, kIOPMAssertionLevelKey));
          if (type && level) {
            if (CFStringCompare(type, CFSTR("PreventUserIdleDisplaySleep"), 0) ==
                kCFCompareEqualTo) {
              IOPMAssertionLevel lvl = kIOPMAssertionLevelOff;
              if (CFNumberGetValue(level, kCFNumberIntType, &lvl) &&
                  lvl == kIOPMAssertionLevelOn) {
                inhibited = true;
                break;
              }
            }
          }
        }
      }
      delete[] keys;
      delete[] values;
    }
    CFRelease(assertionsByProcess);
  }
  if (inhibited != m_inhibited)
    qDebug() << "idle: macos inhibitor" << (inhibited ? "active" : "released");
  m_inhibited = inhibited;
}
