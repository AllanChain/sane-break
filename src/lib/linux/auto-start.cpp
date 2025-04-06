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
#include <QString>
#include <QVariant>

#include "config.h"
#include "core/preferences.h"

void AutoStart::setEnabled(bool enabled) {
#ifdef LINUX_DIST_FLATPAK
  QDBusConnection sessionBus = QDBusConnection::sessionBus();
  QDBusMessage msg = QDBusMessage::createMethodCall(
      "org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
      "org.freedesktop.portal.Background", "RequestBackground");
  int token = QRandomGenerator::global()->bounded(1000, 9999);
  QMap<QString, QVariant> options = {
      {"autostart", enabled},
      {"background", enabled},
      {"commandline", QStringList({"sane-break"})},
      {"reason", "Launch Sane Break at startup"},
      {"handle_token", QString("io/github/AllanChain/SaneBreak/%1").arg(token)}};
  msg << "" << options;
  QDBusMessage response = sessionBus.call(msg);

  if (response.type() == QDBusMessage::ErrorMessage) {
    emit operationResult(false, response.errorMessage());
    return;
  } else if (response.type() != QDBusMessage::ReplyMessage) {
    emit operationResult(false, tr("Unknown error"));
    return;
  }

  QDBusObjectPath handle = response.arguments().at(0).value<QDBusObjectPath>();
  bool connected = sessionBus.connect("org.freedesktop.portal.Desktop", handle.path(),
                                      "org.freedesktop.portal.Request", "Response",
                                      this, SLOT(flatpakCallback(uint, QVariantMap)));
  if (!connected) {
    emit operationResult(false, tr("Failed to connect to Flatpak response"));
    return;
  }
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
      emit operationResult(true);
    } else {
      emit operationResult(false, tr("File not writable"));
    }
  } else {
    if (QFile::remove(desktopPath))
      emit operationResult(true);
    else
      emit operationResult(false, tr("File not writable"));
  }
#endif
}

bool AutoStart::isEnabled(SanePreferences* preferences) {
#ifdef LINUX_DIST_FLATPAK
  return preferences->autoStart;
#endif
  QFile file(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) +
             "/autostart/sane-break.desktop");
  return file.exists();
}

#ifdef LINUX_DIST_FLATPAK
void AutoStart::flatpakCallback(uint response, const QVariantMap& results) {
  if (response > 0)
    emit operationResult(false, tr("The request to autostart was cancelled."));
  else
    emit operationResult(true);
}
#endif
