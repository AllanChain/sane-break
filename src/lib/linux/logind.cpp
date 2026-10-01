// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logind.h"

#include <QByteArray>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QString>
#include <QtGlobal>

namespace {
constexpr char kLogindService[] = "org.freedesktop.login1";
constexpr char kLogindManagerPath[] = "/org/freedesktop/login1";
constexpr char kLogindManagerInterface[] = "org.freedesktop.login1.Manager";
// logind's alias for "the session of the calling process".
constexpr char kLogindAutoSessionPath[] = "/org/freedesktop/login1/session/auto";
}  // namespace

QString LinuxLogind::sessionPath(const QDBusConnection& bus) {
  const QByteArray sessionId = qgetenv("XDG_SESSION_ID");
  if (sessionId.isEmpty()) return QString(kLogindAutoSessionPath);

  QDBusInterface manager(kLogindService, kLogindManagerPath, kLogindManagerInterface,
                         bus);
  // Let logind build the path: a session id is not an object path component as it is
  // ("2" is encoded as "_32").
  QDBusReply<QDBusObjectPath> reply =
      manager.call("GetSession", QString::fromLocal8Bit(sessionId));
  // An error here means the id is stale or the session is not ours to query; the "auto"
  // alias still works for a client that is inside the session it wants to read.
  return reply.isValid() ? reply.value().path() : QString(kLogindAutoSessionPath);
}
