// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDBusConnection>
#include <QObject>

// Detects active idle/display inhibitors via logind on the system bus.
//
// logind subsumes the GNOME SessionManager inhibit path (GNOME forwards its
// inhibitors to logind) and is DE-agnostic, so the same code covers GNOME-Wayland,
// wlroots/KDE Wayland (D-Bus callers) and X11. Used as the cached bool flag feeding
// IdleReader for polling backends.
//
// The inhibitor *list* (`ListInhibitors`) is not polkit-gated; in Flatpak it needs
// `--system-talk-name=org.freedesktop.login1`. We subscribe to the `BlockInhibited`
// property (a colon-separated string of active block-lock types) which emits
// PropertiesChanged on change, then re-scan ListInhibitors — event-driven, no polling.
class LogindInhibitor : public QObject {
  Q_OBJECT
 public:
  explicit LogindInhibitor(QObject* parent = nullptr);
  // Cached flag: is there at least one active block-lock inhibitor whose `what`
  // contains "idle"? Refreshed from the BlockInhibited property and ListInhibitors.
  bool isIdleInhibited() const { return m_inhibited; }

 private slots:
  void refresh();

 private:
  bool m_inhibited = false;
};
