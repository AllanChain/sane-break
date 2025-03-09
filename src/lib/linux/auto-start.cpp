// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "auto-start.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QFile>
#include <QIODevice>
#include <QMap>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QVariant>

#include "config.h"

bool setAutoStartEnabled(bool enabled) {
#ifdef LINUX_DIST_FLATPAK
  return false;
#else
  QString desktopPath =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
      "/autostart/sane-break.desktop";
  QFile desktopFile(":/sane-break.desktop");
  desktopFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QString contents = desktopFile.readAll() +
                     "X-GNOME-Autostart-enabled=true\n"
                     "X-GNOME-Autostart-Delay=2\n"
                     "X-KDE-autostart-phase=2\n"
                     "X-KDE-autostart-after=panel\n"
                     "X-LXQt-Need-Tray=true\n";
  if (enabled) {
    QFile file(desktopPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      file.write(contents.toUtf8());
      file.close();
      return true;
    } else {
      return false;
    }
  } else {
    return QFile::remove(desktopPath);
  }
  return true;
#endif
}

bool autoStartEnabled() {
#ifdef LINUX_DIST_FLATPAK
  return false;
#endif
  QFile file(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
             "/autostart/sane-break.desktop");
  return file.exists();
}
