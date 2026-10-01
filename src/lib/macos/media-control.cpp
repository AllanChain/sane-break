// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "media-control.h"

#include <dlfcn.h>

#include <QDebug>

namespace {
// Values from the private MediaRemote.framework's MRMediaRemoteCommand enum:
// MRMediaRemoteCommandPlay = 0, MRMediaRemoteCommandPause = 1, ...
constexpr int kMRMediaRemoteCommandPause = 1;
// Boolean MRMediaRemoteSendCommand(MRMediaRemoteCommand command,
//                                  NSDictionary *userInfo);
using MRMediaRemoteSendCommandFunc = unsigned char (*)(int, void*);
}  // namespace

void pauseAllMedia() {
  // MediaRemote is a private framework with no public replacement for controlling
  // other apps. Loading it dynamically mirrors lib/macos/screen-lock.cpp and avoids
  // a link-time dependency on an unsupported framework.
  void* handle = dlopen(
      "/System/Library/PrivateFrameworks/MediaRemote.framework/MediaRemote", RTLD_LAZY);
  if (!handle) {
    qWarning() << "MediaRemote.framework unavailable:" << dlerror();
    return;
  }
  auto sendCommand = reinterpret_cast<MRMediaRemoteSendCommandFunc>(
      dlsym(handle, "MRMediaRemoteSendCommand"));
  if (!sendCommand) {
    qWarning() << "MRMediaRemoteSendCommand not found:" << dlerror();
    dlclose(handle);
    return;
  }
  // Since macOS 15.4 the framework requires an entitlement that third-party apps
  // cannot get, so this call is silently denied there. Best effort only.
  if (!sendCommand(kMRMediaRemoteCommandPause, nullptr)) {
    qWarning() << "MediaRemote refused the pause command (macOS 15.4+ requires the "
                  "Media & Apple Music entitlement).";
  }
  dlclose(handle);
}
