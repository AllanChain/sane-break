// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"

#include <qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
#include <QtGui/qpa/qplatformnativeinterface.h>
#else
#include <qguiapplication_platform.h>
#endif
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <QGuiApplication>
#include <cstdint>
#include <cstring>

void registryHandler(void* data, wl_registry*, uint32_t, const char* interface,
                     uint32_t) {
  WaylandCheck* self = static_cast<WaylandCheck*>(data);
  if (strcmp(interface, "zwlr_layer_shell_v1") == 0) self->m_layerShell = true;
  if (strcmp(interface, "ext_idle_notifier_v1") == 0) self->m_idleNotify = true;
}

void registryRemover(void*, wl_registry*, uint32_t) {}

const wl_registry_listener registryListener = {registryHandler, registryRemover};

WaylandCheck::WaylandCheck() {
  if (QGuiApplication::platformName() == "wayland") {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    QNativeInterface::QWaylandApplication* waylandApp =
        qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (!waylandApp) return;
    wl_display* display = waylandApp->display();
#else
    QPlatformNativeInterface* nativeInterface = qGuiApp->platformNativeInterface();
    if (!nativeInterface) return;
    wl_display* display = static_cast<wl_display*>(
        nativeInterface->nativeResourceForIntegration("display"));
#endif
    wl_registry* registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registryListener, this);
    wl_display_roundtrip(display);
    wl_registry_destroy(registry);
  }
}
