// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lock-status.h"

#include <QByteArray>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QtGlobal>

namespace {
constexpr char kLogindService[] = "org.freedesktop.login1";
constexpr char kLogindManagerPath[] = "/org/freedesktop/login1";
constexpr char kLogindManagerInterface[] = "org.freedesktop.login1.Manager";
constexpr char kLogindSessionInterface[] = "org.freedesktop.login1.Session";
// logind's alias for "the session of the calling process".
constexpr char kLogindAutoSessionPath[] = "/org/freedesktop/login1/session/auto";
constexpr char kLockedHintProperty[] = "LockedHint";
constexpr char kPropertiesInterface[] = "org.freedesktop.DBus.Properties";
}  // namespace

LinuxScreenLockStatus::LinuxScreenLockStatus(QObject* parent)
    : ScreenLockStatus(parent) {}

QString LinuxScreenLockStatus::logindSessionPath(const QDBusConnection& bus) {
  const QByteArray sessionId = qgetenv("XDG_SESSION_ID");
  if (sessionId.isEmpty()) return QString(kLogindAutoSessionPath);

  QDBusInterface manager(kLogindService, kLogindManagerPath, kLogindManagerInterface,
                         bus);
  // Let logind build the path: a session id is not an object path component as it is
  // ("2" is encoded as "_32").
  QDBusReply<QDBusObjectPath> reply =
      manager.call("GetSession", QString::fromLocal8Bit(sessionId));
  // An error here means the id is stale or the session is not ours to query; the
  // "auto" alias still works for a client that is inside the session it wants to read.
  return reply.isValid() ? reply.value().path() : QString(kLogindAutoSessionPath);
}

bool LinuxScreenLockStatus::bind(QDBusConnection& bus) {
  if (!bus.isConnected()) return false;
  auto* busInterface = bus.interface();
  // Without this check, probing a session that has no logind would print a D-Bus error
  // on every poll.
  if (!busInterface || !busInterface->isServiceRegistered(kLogindService).value())
    return false;

  m_path = logindSessionPath(bus);
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
