// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "factory.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QGuiApplication>
#include <QObject>
#include <QPluginLoader>

#include "core/idle-time.h"
#include "idle/idle-interface.h"
#include "idle/read-based-idle.h"
#include "inhibitor-proxy-idle.h"
#include "linux-idle-inhibitor.h"

#define SANE_BREAK_LIB_PREFIX "libsanebreak_idle_"

SystemIdleTime* createLinuxIdleTimer(QObject* parent) {
  QString pluginName;
  if (QGuiApplication::platformName() == "wayland") {
    QDBusInterface mutter(
        "org.gnome.Mutter.IdleMonitor", "/org/gnome/Mutter/IdleMonitor/Core",
        "org.gnome.Mutter.IdleMonitor", QDBusConnection::sessionBus());
    pluginName = mutter.isValid() ? "gnome" : "wayland";
  } else {
    pluginName = "x11";
  }

  QPluginLoader loader(SANE_BREAK_LIB_PREFIX + pluginName);
  if (!loader.load()) {
    qCritical() << loader.errorString();
    qFatal("Fail to load %s idle time plugin.", qPrintable(pluginName));
  }

  auto* logind = new LinuxIdleInhibitor(parent);

  // Reader plugins (GNOME, X11) expose a polling interface; the Wayland plugin is
  // event-driven and uses the proxy wrapper instead.
  if (auto* reader = qobject_cast<IdleTimeReaderInterface*>(loader.instance())) {
    reader->setParent(parent);
    auto* rbt = new ReadBasedIdleTime(parent, [reader]() { return reader->read(); });
    rbt->setInhibitor([logind, reader]() {
      return logind->isIdleInhibited() || reader->isInhibited();
    });
    return rbt;
  }

  if (auto* reader = qobject_cast<IdleTimeInterface*>(loader.instance())) {
    reader->setParent(parent);
    return new InhibitorProxyIdleTime(reader, logind, parent);
  }

  qFatal("Fail to instantiate %s idle time plugin.", qPrintable(pluginName));
}
