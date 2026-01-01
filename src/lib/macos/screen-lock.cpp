// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <dlfcn.h>

#include <QDebug>
#include <QProcess>

bool lockScreenWithFramework() {
  void* handle =
      dlopen("/System/Library/PrivateFrameworks/login.framework/login", RTLD_LAZY);
  if (!handle) {
    qWarning() << "Error opening library: " << dlerror();
    return false;
  }
  typedef void (*SACLockScreenImmediateFunc)();
  SACLockScreenImmediateFunc func =
      (SACLockScreenImmediateFunc)dlsym(handle, "SACLockScreenImmediate");
  const char* dlsym_error = dlerror();
  if (dlsym_error) {
    qWarning() << "Error getting symbol: " << dlsym_error;
    dlclose(handle);
    return false;
  }
  func();
  dlclose(handle);
  return true;
}

bool lockScreenWithSleep() {
  QProcess process;
  process.start("pmset", {"displaysleepnow"});
  process.waitForFinished();
  return process.exitCode() == 0;
}

bool lockScreen() {
  if (!lockScreenWithFramework()) return lockScreenWithSleep();
  return true;
}
