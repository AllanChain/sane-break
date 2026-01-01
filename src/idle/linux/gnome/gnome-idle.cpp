// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gnome-idle.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

#include "idle/idle-interface.h"

IdleTimeGNOME::IdleTimeGNOME() {
  iface = new QDBusInterface(
      "org.gnome.Mutter.IdleMonitor", "/org/gnome/Mutter/IdleMonitor/Core",
      "org.gnome.Mutter.IdleMonitor", QDBusConnection::sessionBus());
}

int IdleTimeGNOME::read() {
  QDBusReply<qulonglong> reply = iface->call("GetIdletime");
  return reply.value();
}

IdleTimeGNOME::~IdleTimeGNOME() {
  if (iface != nullptr) delete iface;
}
