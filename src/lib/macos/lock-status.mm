// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lock-status.h"

#import <ApplicationServices/ApplicationServices.h>
#import <CoreFoundation/CoreFoundation.h>

#include <QMetaObject>

namespace {

// loginwindow posts these when the screen lock goes up and down. They are not trusted
// for state (see the header): the session dictionary below stays the single answer.
//
// const, not constexpr: CFSTR casts the string-literal builtin to CFStringRef through a
// reinterpret-cast-like conversion, which C++ forbids in a constant expression.
const CFStringRef kLockedNotification = CFSTR("com.apple.screenIsLocked");
const CFStringRef kUnlockedNotification = CFSTR("com.apple.screenIsUnlocked");

void lockNotificationReceived(CFNotificationCenterRef /*center*/, void* observer,
                              CFStringRef /*name*/, const void* /*object*/,
                              CFDictionaryRef /*userInfo*/) {
  auto* self = static_cast<DarwinScreenLockStatus*>(observer);
  // Queued instead of called inline: this runs inside CoreFoundation's notification
  // dispatch, and the state read that follows is a synchronous round trip.
  QMetaObject::invokeMethod(self, [self] { self->check(); }, Qt::QueuedConnection);
}

}  // namespace

DarwinScreenLockStatus::DarwinScreenLockStatus(QObject* parent)
    : ScreenLockStatus(parent) {
  CFNotificationCenterRef center = CFNotificationCenterGetDistributedCenter();
  // DeliverImmediately: the default suspension behaviour coalesces distributed
  // notifications while the app is inactive, and a break app spends most of its life in
  // the background.
  CFNotificationCenterAddObserver(center, this, lockNotificationReceived,
                                  kLockedNotification, nullptr,
                                  CFNotificationSuspensionBehaviorDeliverImmediately);
  CFNotificationCenterAddObserver(center, this, lockNotificationReceived,
                                  kUnlockedNotification, nullptr,
                                  CFNotificationSuspensionBehaviorDeliverImmediately);
}

DarwinScreenLockStatus::~DarwinScreenLockStatus() {
  CFNotificationCenterRemoveEveryObserver(CFNotificationCenterGetDistributedCenter(),
                                          this);
}

bool DarwinScreenLockStatus::systemScreenLocked() {
  // The current session dictionary carries the lock state as the boolean
  // CGSSessionScreenIsLocked. Absent key means unlocked - that is how an unlocked
  // session reports it, not a failure to read.
  //
  // CGSessionCopyCurrentDictionary is marked deprecated but has no replacement for lock
  // state; keep the call local so a future removal shows up as one warning here rather
  // than a build break everywhere.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  CFDictionaryRef session = CGSessionCopyCurrentDictionary();
#pragma clang diagnostic pop
  if (!session) return false;

  bool locked = false;
  CFTypeRef value = CFDictionaryGetValue(session, CFSTR("CGSSessionScreenIsLocked"));
  if (value && CFGetTypeID(value) == CFBooleanGetTypeID()) {
    locked = CFBooleanGetValue((CFBooleanRef)value);
  } else if (value && CFGetTypeID(value) == CFNumberGetTypeID()) {
    // The key has been observed as a number as well; both spellings mean the same.
    SInt32 number = 0;
    CFNumberGetValue((CFNumberRef)value, kCFNumberSInt32Type, &number);
    locked = number != 0;
  }
  CFRelease(session);
  return locked;
}
