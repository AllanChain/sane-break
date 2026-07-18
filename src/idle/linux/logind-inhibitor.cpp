// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "logind-inhibitor.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <Qt>
#include <tuple>

// Each inhibitor is (what, who, why, mode, pid, uid).
using InhibitorTuple = std::tuple<QString, QString, QString, QString, uint, uint>;

LogindInhibitor::LogindInhibitor(QObject* parent) : QObject(parent) {
  QDBusConnection bus = QDBusConnection::systemBus();
  // We only need the service if it's actually on the bus; otherwise refresh() is a
  // no-op and we stay "not inhibited".
  if (!bus.interface()->isServiceRegistered("org.freedesktop.login1")) return;

  // Re-scan whenever the set of active block locks changes (event-driven, no polling).
  bus.connect("org.freedesktop.login1", "/org/freedesktop/login1",
              "org.freedesktop.DBus.Properties", "PropertiesChanged", this,
              SLOT(refresh()));
  refresh();
}

void LogindInhibitor::refresh() {
  QDBusInterface login1("org.freedesktop.login1", "/org/freedesktop/login1",
                        "org.freedesktop.login1.Manager", QDBusConnection::systemBus());

  bool inhibited = false;

  // Prefer the per-inhibitor list: an inhibitor is relevant iff `what` contains
  // "idle" and it is a "block" lock (delay locks don't prevent idle).
  QDBusReply<QList<InhibitorTuple>> reply = login1.call("ListInhibitors");
  if (reply.isValid()) {
    for (const InhibitorTuple& inhibitor : reply.value()) {
      const QString& what = std::get<0>(inhibitor);
      const QString& mode = std::get<3>(inhibitor);
      if (what.split(u':').contains("idle", Qt::CaseSensitive) && mode == "block") {
        inhibited = true;
        break;
      }
    }
  } else {
    // ListInhibitors unavailable (old systemd): fall back to the BlockInhibited
    // property, a colon-separated string of active block-lock types.
    QDBusReply<QDBusVariant> prop =
        login1.call("Get", "org.freedesktop.login1.Manager", "BlockInhibited");
    if (prop.isValid()) {
      inhibited = prop.value().variant().toString().split(u':').contains(
          "idle", Qt::CaseSensitive);
    }
  }

  if (inhibited != m_inhibited)
    qDebug() << "idle: logind inhibitor" << (inhibited ? "active" : "released");
  m_inhibited = inhibited;
}
