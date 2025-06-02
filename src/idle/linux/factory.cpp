// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
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

#define SANE_BREAK_LIB_PREFIX "libsanebreak_idle_"

SystemIdleTime* createLinuxIdleTimer(QObject* parent) {
  if (QGuiApplication::platformName() == "wayland") {
    QDBusInterface iface("org.gnome.Mutter.IdleMonitor",
                         "/org/gnome/Mutter/IdleMonitor/Core",
                         "org.gnome.Mutter.IdleMonitor", QDBusConnection::sessionBus());
    if (iface.isValid()) {
      QPluginLoader loader(SANE_BREAK_LIB_PREFIX "gnome");
      if (!loader.load()) {
        qCritical() << loader.errorString();
        qFatal("Fail to find GNOME idle time plugin.");
      }
      auto reader = qobject_cast<IdleTimeReaderInterface*>(loader.instance());
      if (!reader) qFatal("Fail to load GNOME idle time plugin.");
      reader->setParent(parent);
      return new ReadBasedIdleTime(parent, [reader]() { return reader->read(); });
    }
    QPluginLoader loader(SANE_BREAK_LIB_PREFIX "wayland");
    if (!loader.load()) {
      qCritical() << loader.errorString();
      qFatal("Fail to find Wayland idle time plugin.");
    }
    auto reader = qobject_cast<IdleTimeInterface*>(loader.instance());
    if (!reader) qFatal("Fail to load Wayland idle time plugin.");
    reader->setParent(parent);
    return reader;
  } else {
    QPluginLoader loader(SANE_BREAK_LIB_PREFIX "x11");
    if (!loader.load()) {
      qCritical() << loader.errorString();
      qFatal("Fail to find X11 idle time plugin.");
    }
    auto reader = qobject_cast<IdleTimeReaderInterface*>(loader.instance());
    if (!reader) qFatal("Fail to load X11 idle time plugin.");
    reader->setParent(parent);
    return new ReadBasedIdleTime(parent, [reader]() { return reader->read(); });
  }
}
