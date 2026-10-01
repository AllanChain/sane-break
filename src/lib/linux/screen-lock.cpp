// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "screen-lock.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QProcess>

#include "logind.h"

namespace {
constexpr char kLogindService[] = "org.freedesktop.login1";
constexpr char kLogindSessionInterface[] = "org.freedesktop.login1.Session";

// Asks logind to lock this process' session. logind only emits the session's "Lock"
// signal for the desktop's screen locker to act on, but that is the same path
// `loginctl lock-session` takes and it stays inside the sandbox, unlike the session-bus
// screen saver xdg-screensaver reaches for.
bool lockScreenWithLogind() {
  QDBusConnection bus = QDBusConnection::systemBus();
  if (!bus.isConnected()) return false;
  auto* busInterface = bus.interface();
  // Probing a system without logind would print a D-Bus error on every call.
  if (!busInterface || !busInterface->isServiceRegistered(kLogindService).value())
    return false;

  QDBusMessage message = QDBusMessage::createMethodCall(
      kLogindService, LinuxLogind::sessionPath(bus), kLogindSessionInterface, "Lock");
  const QDBusMessage reply = bus.call(message);
  return reply.type() != QDBusMessage::ErrorMessage;
}

bool lockScreenWithXdgScreensaver() {
  QProcess process;
  process.start("xdg-screensaver", {"lock"});
  process.waitForFinished();
  return process.exitCode() == 0;
}
}  // namespace

bool lockScreen() {
  if (lockScreenWithLogind()) return true;
  // No logind (a non-systemd system without elogind): fall back to the legacy xdg-utils
  // script, which asks the desktop over the session bus. It is not present in a Flatpak
  // sandbox, where logind is the way in.
  return lockScreenWithXdgScreensaver();
}
