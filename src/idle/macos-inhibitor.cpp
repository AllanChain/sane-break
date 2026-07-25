// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "macos-inhibitor.h"

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/pwr_mgt/IOPMLib.h>

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
  // This is the system-wide aggregate used by `pmset -g assertions`.
  CFDictionaryRef assertionStatus = nullptr;
  IOReturn result = IOPMCopyAssertionsStatus(&assertionStatus);
  if (result == kIOReturnSuccess && assertionStatus != nullptr) {
    CFNumberRef level = static_cast<CFNumberRef>(
        CFDictionaryGetValue(assertionStatus, CFSTR("PreventUserIdleDisplaySleep")));
    int levelValue = 0;
    if (level && CFNumberGetValue(level, kCFNumberIntType, &levelValue)) {
      // IOPMCopyAssertionsStatus reports a nonzero aggregate state (currently 1),
      // unlike per-process assertions, which use kIOPMAssertionLevelOn (255).
      inhibited = levelValue != 0;
    }
    CFRelease(assertionStatus);
  }
  m_inhibited = inhibited;
}
