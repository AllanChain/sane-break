// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "media-control.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QString>
#include <QStringList>

namespace {
constexpr char kMprisPrefix[] = "org.mpris.MediaPlayer2";
constexpr char kMprisPath[] = "/org/mpris/MediaPlayer2";
constexpr char kPlayerInterface[] = "org.mpris.MediaPlayer2.Player";
}  // namespace

void pauseAllMedia() {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.isConnected()) return;
  QDBusConnectionInterface* busInterface = bus.interface();
  if (!busInterface) return;

  const QStringList services = busInterface->registeredServiceNames().value();
  for (const QString& service : services) {
    if (!service.startsWith(QLatin1String(kMprisPrefix))) continue;
    // Every MPRIS player shares the same object path and Player interface. Pause is
    // a no-op when playback is already stopped or paused, so calling it on every
    // player is safe. NoBlock keeps this from stalling the GUI thread.
    QDBusInterface player(service, QLatin1String(kMprisPath),
                          QLatin1String(kPlayerInterface), bus);
    if (player.isValid()) player.call(QDBus::NoBlock, QStringLiteral("Pause"));
  }
}
