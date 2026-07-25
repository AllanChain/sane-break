// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDBusConnection>
#include <QObject>

// Detects active idle inhibitors on Linux via two complementary D-Bus sources:
//
// 1. logind (system bus): ListInhibitors() / BlockInhibited property.
//    Catches GNOME-forwarded SessionManager/ScreenSaver inhibitors and direct
//    systemd-inhibit calls. GNOME-specific for ScreenSaver forwarding — KDE's
//    PowerDevil does NOT forward idle inhibitors to logind.
//
// 2. KDE PowerDevil PolicyAgent (session bus): ActiveInhibitions property.
//    Catches portal-based inhibitors (Firefox, Chromium) on KDE. xdg-desktop-portal-kde
//    routes Inhibit calls to PolicyAgent.AddInhibition, not to ScreenSaver or logind.
//    The ActiveInhibitions property emits PropertiesChanged — event-driven, no polling.
//
// Both sources are queried in refresh() and ORed together. On GNOME only logind is
// active (PowerDevil absent); on KDE both are active but only PolicyAgent catches
// app idle inhibitors; on wlroots only logind is active (portal Inhibit unimplemented).
class LinuxIdleInhibitor : public QObject {
  Q_OBJECT
 public:
  explicit LinuxIdleInhibitor(QObject* parent = nullptr);
  bool isIdleInhibited() const { return m_inhibited; }

 signals:
  void inhibitorChanged(bool inhibited);

 private slots:
  void refresh();

 private:
  bool m_inhibited = false;
  bool checkLogind();
  bool checkKDE();
};
