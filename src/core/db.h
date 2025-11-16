// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

class BreakDatabase : public QObject {
  Q_OBJECT
 public:
  BreakDatabase(QSqlDatabase, QObject* parent = nullptr);
  ~BreakDatabase();
  static QString dbPath();
  QSqlError logEvent(const QString& eventType, const QJsonObject& eventData = {});

 protected:
  QSqlError ensureDb();
  QSqlDatabase m_db;
};
