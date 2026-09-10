// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDBusConnection>
#include <QDBusInterface>
#include <QObject>
#include <QString>

#include "screen-lock-status.h"  // IWYU pragma: export

// Reads logind's per-session LockedHint, the only lock state a Linux desktop session
// publishes for others to read. The lock screen owns that hint (kscreenlocker,
// gnome-shell), so a display that merely blanked keeps it false, and the read behaves
// the same under X11, Wayland and wlroots.
//
// Deliberately not org.freedesktop.ScreenSaver's Active state: that means "the
// screensaver is showing", so it is true on every monitor blank while the session stays
// unlocked, and pausing on it would take breaks away with nothing to explain why.
//
// A session logind does not manage, or a sandbox without talk access to the name,
// answers "not locked" and the feature disables itself.
class LinuxScreenLockStatus : public ScreenLockStatus {
  Q_OBJECT
 public:
  LinuxScreenLockStatus(QObject* parent = nullptr);
  bool systemScreenLocked() override;

 private:
  // Finds this session's LockedHint on the given bus and subscribes to its property
  // changes. A failed subscribe is not fatal: the base class poll covers it.
  bool bind(QDBusConnection& bus);
  // This process' logind session object path, or the "auto" alias when it cannot be
  // resolved.
  QString logindSessionPath(const QDBusConnection& bus);

  QDBusInterface* m_props = nullptr;  // org.freedesktop.DBus.Properties on the session
  QString m_path;
  // No source is normal (non-systemd, sandbox): report it once, not every poll.
  bool m_loggedNoSource = false;
};
