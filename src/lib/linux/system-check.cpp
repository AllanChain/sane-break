// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#include "config.h"

#ifdef Q_OS_LINUX
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QGuiApplication>
#include <QList>
#include <QPluginLoader>
#include <QString>
#include <QSystemTrayIcon>

#include "system-check.h"
#include "wayland-check/interface.h"

namespace LinuxSystemSupport {
bool layerShell = false;
bool idleNotify = false;
bool idleGNOME = false;
bool trayIcon = false;
}  // namespace LinuxSystemSupport

void LinuxSystemSupport::check() {
  LinuxSystemSupport::trayIcon = QSystemTrayIcon::isSystemTrayAvailable();

  QDBusInterface iface("org.gnome.Mutter.IdleMonitor",
                       "/org/gnome/Mutter/IdleMonitor/Core",
                       "org.gnome.Mutter.IdleMonitor", QDBusConnection::sessionBus());
  LinuxSystemSupport::idleGNOME = iface.isValid();

  QPluginLoader loader("libsanebreak_wayland_check");
  if (!loader.load()) {
    qWarning() << loader.errorString();
    return;
  }
  auto* waylandCheck = qobject_cast<WaylandCheckInterface*>(loader.instance());
  if (!waylandCheck) return;
  LinuxSystemSupport::idleNotify = waylandCheck->supportsIdleNotify();
  LinuxSystemSupport::layerShell = waylandCheck->supportsLayerShell();
}

const QList<QString> LinuxSystemSupport::errors() {
  QList<QString> list{};
  if (QGuiApplication::platformName() == "wayland") {
    if (!LinuxSystemSupport::idleGNOME && !LinuxSystemSupport::idleNotify)
      list.append("Idle detection is not available");
  }
  return list;
}
const QList<QString> LinuxSystemSupport::warnings() {
  QList<QString> list{};
  if (!LinuxSystemSupport::trayIcon)
    list.append(QCoreApplication::tr(
        "Tray icon is not available, falling back to a normal window"));
  if (QGuiApplication::platformName() == "wayland") {
    if (!LinuxSystemSupport::layerShell) {
      list.append(QCoreApplication::tr(
          "The compositor does not support layer shell, and window positioning will be "
          "bugged"));
      list.append(QCoreApplication::tr(
          "You need to install the shell extension if you are using GNOME. Otherwise, "
          "Sane break is almost unusable"));
    }
  }
  return list;
}
#endif
