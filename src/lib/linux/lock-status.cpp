// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lock-status.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QString>
#include <QVariant>

#include "logind.h"

namespace {
constexpr char kLogindService[] = "org.freedesktop.login1";
constexpr char kLogindSessionInterface[] = "org.freedesktop.login1.Session";
constexpr char kLockedHintProperty[] = "LockedHint";
constexpr char kPropertiesInterface[] = "org.freedesktop.DBus.Properties";
}  // namespace

LinuxScreenLockStatus::LinuxScreenLockStatus(QObject* parent)
    : ScreenLockStatus(parent) {}

bool LinuxScreenLockStatus::bind(QDBusConnection& bus) {
  if (!bus.isConnected()) return false;
  auto* busInterface = bus.interface();
  // Without this check, probing a session that has no logind would print a D-Bus error
  // on every poll.
  if (!busInterface || !busInterface->isServiceRegistered(kLogindService).value())
    return false;

  m_path = LinuxLogind::sessionPath(bus);
  const QString properties = QString::fromLatin1(kPropertiesInterface);
  auto* props = new QDBusInterface(kLogindService, m_path, properties, bus, this);
  const QDBusReply<QVariant> hint =
      props->call("Get", kLogindSessionInterface, kLockedHintProperty);
  if (!hint.isValid()) {
    delete props;  // Nothing was subscribed yet, so no connection refers to it
    m_path.clear();
    return false;
  }
  m_props = props;
  // Only the fact that something changed matters here, so check() ignores what logind
  // reports and re-reads LockedHint the same way the poll does.
  if (!bus.connect(kLogindService, m_path, properties, "PropertiesChanged", this,
                   SLOT(check()))) {
    // Not fatal: the base class poll notices the change instead, just later.
    qDebug() << "pause: cannot subscribe to" << m_path << "properties";
  }
  qDebug() << "pause: watching screen lock via" << kLogindService << m_path
           << kLockedHintProperty;
  m_loggedNoSource = false;
  return true;
}

bool LinuxScreenLockStatus::systemScreenLocked() {
  if (!m_props) {
    // Retried on every poll until it answers: logind can be unavailable forever, but it
    // can also come up after Sane Break started. A local copy because subscribing to
    // notifications needs a non-const connection.
    QDBusConnection systemBus = QDBusConnection::systemBus();
    if (!bind(systemBus)) {
      if (!m_loggedNoSource) {
        m_loggedNoSource = true;
        qDebug() << "pause: no D-Bus lock source available, screen lock pause disabled";
      }
      return false;
    }
  }

  QDBusReply<QVariant> reply =
      m_props->call("Get", kLogindSessionInterface, kLockedHintProperty);
  if (reply.isValid()) return reply.value().toBool();

  // A failed read (session ended, logind restarted, talk access revoked) is not
  // "unlocked": that would stop pausing on every future lock. Drop the source so the
  // next poll binds a working one, and report unlocked meanwhile.
  qDebug() << "pause: lost" << m_path << "-" << reply.error().message();
  m_props->deleteLater();
  m_props = nullptr;
  m_path.clear();
  return false;
}
