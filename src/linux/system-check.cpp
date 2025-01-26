// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>
#include <qlist.h>

#include "config.h"

#ifdef Q_OS_LINUX
#include <QDBusConnection>
#include <QDBusInterface>
#include <QGuiApplication>
#include <QList>
#include <QString>
#include <QSystemTrayIcon>

#include "system-check.h"
#ifdef ENABLE_WAYLAND
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <QDBusConnection>
#include <QDBusInterface>
#include <cstdint>
#include <cstring>

namespace LinuxSystemSupport {
bool layerShell = false;
bool idleNotify = false;
bool idleGNOME = false;
bool trayIcon = false;
}  // namespace LinuxSystemSupport

void registryHandler(void *data, wl_registry *registry, uint32_t id,
                     const char *interface, uint32_t version) {
  // Layer shell shoud be supported both compile and run time
#ifdef LayerShellQt_FOUND
  if (strcmp(interface, "zwlr_layer_shell_v1") == 0)
    LinuxSystemSupport::layerShell = true;
#endif
  if (strcmp(interface, "ext_idle_notifier_v1") == 0)
    LinuxSystemSupport::idleNotify = true;
}

void registryRemover(void *data, wl_registry *registry, uint32_t id) {}

const wl_registry_listener registryListener = {registryHandler, registryRemover};
#endif  // ENABLE_WAYLAND

void LinuxSystemSupport::check() {
  LinuxSystemSupport::trayIcon = QSystemTrayIcon::isSystemTrayAvailable();

  QDBusInterface iface("org.gnome.Mutter.IdleMonitor",
                       "/org/gnome/Mutter/IdleMonitor/Core",
                       "org.gnome.Mutter.IdleMonitor", QDBusConnection::sessionBus());
  LinuxSystemSupport::idleGNOME = iface.isValid();
#ifdef ENABLE_WAYLAND
  if (QGuiApplication::platformName() == "wayland") {
    QNativeInterface::QWaylandApplication *waylandApp =
        qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    wl_display *display = waylandApp->display();
    wl_registry *registry = wl_display_get_registry(display);
    wl_seat *seat = waylandApp->seat();
    wl_registry_add_listener(registry, &registryListener, nullptr);
    wl_display_roundtrip(display);
    wl_registry_destroy(registry);
  }
#endif
}

const QList<QString> LinuxSystemSupport::errors() {
  QList<QString> list{};
#ifdef ENABLE_WAYLAND
  if (!LinuxSystemSupport::idleGNOME && !LinuxSystemSupport::idleNotify)
    list.append("Idle detection is not available");
#endif
  return list;
}
const QList<QString> LinuxSystemSupport::warnings() {
  QList<QString> list{};
  if (!LinuxSystemSupport::trayIcon)
    list.append("Tray icon is not available, falling back to a normal window");
#ifdef ENABLE_WAYLAND
  if (!LinuxSystemSupport::layerShell) {
#ifdef LayerShellQt_FOUND
    list.append(
        "The compositor does not support layer shell, and window positioning will be "
        "bugged");
#else
    list.append(
        "Sane Break is not compiled with layer shell support, and window positioning "
        "will be bugged");
#endif
    list.append(
        "You need to install the shell extension if you are using GNOME. Otherwise, "
        "Sane break is almost unusable");
  }
#endif
  return list;
}
#endif
