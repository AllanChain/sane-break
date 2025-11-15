// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/db.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLatin1String>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QString>

BreakDatabase::BreakDatabase(QSqlDatabase db, QObject* parent)
    : QObject(parent), m_db(db) {}

BreakDatabase::~BreakDatabase() { m_db.close(); }

QString BreakDatabase::dbPath() {
  QString dataLocation =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir dir(dataLocation);
  if (!dir.exists()) {
    if (!dir.mkpath(".")) {
      qWarning() << "Failed to create database directory:" << dataLocation;
      return QString();  // Return empty path to indicate failure
    }
  }
#ifndef NDEBUG
  return dir.filePath("sane-break-debug.db");
#else
  return dir.filePath("sane-break.db");
#endif
}

QSqlError BreakDatabase::ensureDb() {
  if (m_db.isOpen()) {
    return QSqlError();
  }

  QString path = dbPath();
  if (path.isEmpty()) {
    QSqlError error("Database directory creation failed",
                    "Could not create database directory", QSqlError::ConnectionError);
    qWarning() << "Database directory creation failed:" << error.text();
    return error;
  }

  if (m_db.databaseName().isEmpty()) {
    m_db.setDatabaseName(path);
  }

  if (!m_db.open()) {
    QSqlError error = m_db.lastError();
    qWarning() << "Failed to open database:" << error.text();
    return error;
  }

  static const auto createTableQuery = QLatin1String(
      R"(
        CREATE TABLE IF NOT EXISTS events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            type TEXT NOT NULL,
            data TEXT NOT NULL CHECK (json_valid(data))
        )
    )");
  QSqlQuery query(m_db);
  if (!query.exec(createTableQuery)) {
    QSqlError error = query.lastError();
    qWarning() << "Failed to create table:" << error.text();
    return error;
  }

  return QSqlError();
}

QSqlError BreakDatabase::logEvent(const QString& eventType,
                                  const QJsonObject& eventData) {
  auto err = ensureDb();
  if (err.type() != QSqlError::NoError) {
    qWarning() << "Database not available, skipping event log:" << eventType;
    return err;
  }

  QSqlQuery query(m_db);
  static const auto insertQuery =
      QLatin1String("INSERT INTO events (type, data) VALUES (?, ?)");
  query.prepare(insertQuery);
  query.addBindValue(eventType);
  query.addBindValue(QJsonDocument(eventData).toJson(QJsonDocument::Compact));

  if (!query.exec()) {
    QSqlError error = query.lastError();
    qWarning() << "Failed to insert event:" << eventType << error.text();
    return error;
  }

  return QSqlError();
}
