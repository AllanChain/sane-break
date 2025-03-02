// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "battery.h"

#include <qglobal.h>

#ifdef Q_OS_MACOS
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/ps/IOPowerSources.h>

bool DarwinBatteryStatus::systemOnBattery() {
  // Get the power source information
  CFTypeRef powerSourceInfo = IOPSCopyPowerSourcesInfo();
  CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(powerSourceInfo);

  if (powerSourcesList) {
    for (CFIndex i = 0; i < CFArrayGetCount(powerSourcesList); i++) {
      CFTypeRef powerSource = CFArrayGetValueAtIndex(powerSourcesList, i);
      CFDictionaryRef powerSourceDescription =
          IOPSGetPowerSourceDescription(powerSourceInfo, powerSource);

      if (powerSourceDescription) {
        CFStringRef powerSourceState = (CFStringRef)CFDictionaryGetValue(
            powerSourceDescription, CFSTR(kIOPSPowerSourceStateKey));

        if (CFEqual(powerSourceState, CFSTR(kIOPSACPowerValue))) {
          return false;
        } else if (CFEqual(powerSourceState, CFSTR(kIOPSBatteryPowerValue))) {
          return true;
        }
      }
    }
    CFRelease(powerSourcesList);
  }
  CFRelease(powerSourceInfo);

  return false;
}
#endif
