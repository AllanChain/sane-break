// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "linux-idle-inhibitor.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusVariant>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QStringList>
#include <Qt>
// logind inhibitor struct: (what, who, why, mode, pid, uid).
// KDE PolicyAgent inhibition struct: (what, who, why, mode, flags).
// QDBusArgument has no operator>> for std::tuple (only std::pair), so we parse
// the D-Bus structs manually via beginArray/beginStructure.

LinuxIdleInhibitor::LinuxIdleInhibitor(QObject* parent) : QObject(parent) {
  QDBusConnection system = QDBusConnection::systemBus();
  if (system.interface()->isServiceRegistered("org.freedesktop.login1")) {
    system.connect("org.freedesktop.login1", "/org/freedesktop/login1",
                   "org.freedesktop.DBus.Properties", "PropertiesChanged", this,
                   SLOT(refresh()));
  }

  QDBusConnection session = QDBusConnection::sessionBus();
  if (session.interface()->isServiceRegistered("org.kde.Solid.PowerManagement")) {
    session.connect(
        "org.kde.Solid.PowerManagement", "/org/kde/Solid/PowerManagement/PolicyAgent",
        "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(refresh()));
  }

  refresh();
}

void LinuxIdleInhibitor::refresh() {
  bool inhibited = checkLogind() || checkKDE();

  if (inhibited != m_inhibited) {
    qDebug() << "idle: inhibitor" << (inhibited ? "active" : "released");
    m_inhibited = inhibited;
    emit inhibitorChanged(m_inhibited);
  }
}

bool LinuxIdleInhibitor::checkLogind() {
  QDBusInterface login1("org.freedesktop.login1", "/org/freedesktop/login1",
                        "org.freedesktop.login1.Manager", QDBusConnection::systemBus());

  // ListInhibitors returns a(ssssuu) — an array of structs, each with
  // (what, who, why, mode, pid, uid). Parse manually since QDBusArgument
  // has no operator>> for std::tuple with >2 elements.
  QDBusMessage reply = login1.call("ListInhibitors");
  if (reply.type() == QDBusMessage::ReplyMessage && !reply.arguments().isEmpty()) {
    const QDBusArgument arg = reply.arguments().at(0).value<QDBusArgument>();
    arg.beginArray();
    while (!arg.atEnd()) {
      arg.beginStructure();
      QString what, who, why, mode;
      uint pid, uid;
      arg >> what >> who >> why >> mode >> pid >> uid;
      arg.endStructure();
      if (what.split(u':').contains("idle", Qt::CaseSensitive) && mode == "block")
        return true;
    }
    arg.endArray();
  } else {
    // ListInhibitors unavailable (old systemd): fall back to the BlockInhibited
    // property. Must use org.freedesktop.DBus.Properties for Get.
    QDBusInterface props("org.freedesktop.login1", "/org/freedesktop/login1",
                         "org.freedesktop.DBus.Properties",
                         QDBusConnection::systemBus());
    QDBusReply<QDBusVariant> prop =
        props.call("Get", "org.freedesktop.login1.Manager", "BlockInhibited");
    if (prop.isValid())
      return prop.value().variant().toString().split(u':').contains("idle",
                                                                    Qt::CaseSensitive);
  }
  return false;
}

bool LinuxIdleInhibitor::checkKDE() {
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.interface()->isServiceRegistered("org.kde.Solid.PowerManagement"))
    return false;

  // ActiveInhibitions is a property; must read it via org.freedesktop.DBus.Properties,
  // not via the PolicyAgent interface (Get belongs to Properties, not PolicyAgent).
  QDBusInterface props("org.kde.Solid.PowerManagement",
                       "/org/kde/Solid/PowerManagement/PolicyAgent",
                       "org.freedesktop.DBus.Properties", bus);
  QDBusReply<QDBusVariant> reply = props.call(
      "Get", "org.kde.Solid.PowerManagement.PolicyAgent", "ActiveInhibitions");
  if (!reply.isValid()) return false;

  // ActiveInhibitions is a(ssssu) — an array of structs, each with
  // (what, who, why, mode, flags). Parse manually.
  const QDBusArgument arg = reply.value().variant().value<QDBusArgument>();
  arg.beginArray();
  while (!arg.atEnd()) {
    arg.beginStructure();
    QString what, who, why, mode;
    uint flags;
    arg >> what >> who >> why >> mode >> flags;
    arg.endStructure();
    if (what.split(u':').contains("idle", Qt::CaseSensitive) && mode == "block")
      return true;
  }
  arg.endArray();
  return false;
}
