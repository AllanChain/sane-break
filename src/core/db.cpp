// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/db.h"

#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLatin1String>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QString>
#include <QVariant>
#include <Qt>
#include <algorithm>

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

  static const auto createEventsTable = QLatin1String(
      R"(
        CREATE TABLE IF NOT EXISTS events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            type TEXT NOT NULL,
            data TEXT NOT NULL CHECK (json_valid(data))
        )
    )");
  QSqlQuery query(m_db);
  if (!query.exec(createEventsTable)) {
    QSqlError error = query.lastError();
    qWarning() << "Failed to create events table:" << error.text();
    return error;
  }

  static const auto createSpansTable = QLatin1String(
      R"(
        CREATE TABLE IF NOT EXISTS spans (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT NOT NULL,
            started_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
            ended_at TIMESTAMP,
            data TEXT NOT NULL DEFAULT '{}' CHECK (json_valid(data))
        )
    )");
  if (!query.exec(createSpansTable)) {
    QSqlError error = query.lastError();
    qWarning() << "Failed to create spans table:" << error.text();
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

int BreakDatabase::openSpan(const QString& type, const QJsonObject& data,
                            const QDateTime& startTime) {
  auto err = ensureDb();
  if (err.type() != QSqlError::NoError) {
    qWarning() << "Database not available, skipping openSpan:" << type;
    return -1;
  }

  QSqlQuery query(m_db);
  if (startTime.isValid()) {
    query.prepare(
        "INSERT INTO spans (type, started_at, data) VALUES (?, ?, ?)");
    query.addBindValue(type);
    query.addBindValue(startTime.toUTC().toString(Qt::ISODate));
    query.addBindValue(QJsonDocument(data).toJson(QJsonDocument::Compact));
  } else {
    query.prepare("INSERT INTO spans (type, data) VALUES (?, ?)");
    query.addBindValue(type);
    query.addBindValue(QJsonDocument(data).toJson(QJsonDocument::Compact));
  }

  if (!query.exec()) {
    qWarning() << "Failed to open span:" << type << query.lastError().text();
    return -1;
  }

  return query.lastInsertId().toInt();
}

void BreakDatabase::closeSpan(int spanId, const QJsonObject& extraData,
                              const QDateTime& endTime) {
  if (spanId < 0) return;

  auto err = ensureDb();
  if (err.type() != QSqlError::NoError) {
    qWarning() << "Database not available, skipping closeSpan:" << spanId;
    return;
  }

  QSqlQuery query(m_db);
  QString endTimeStr =
      endTime.isValid() ? endTime.toUTC().toString(Qt::ISODate) : QString();

  if (extraData.isEmpty()) {
    query.prepare(
        "UPDATE spans SET ended_at = COALESCE(ended_at, ?) WHERE id = ?");
    query.addBindValue(
        endTimeStr.isEmpty() ? QVariant(QDateTime::currentDateTimeUtc().toString(
                                   Qt::ISODate))
                             : QVariant(endTimeStr));
    query.addBindValue(spanId);
  } else {
    query.prepare(
        "UPDATE spans SET ended_at = COALESCE(ended_at, ?), "
        "data = json_patch(data, ?) WHERE id = ?");
    query.addBindValue(
        endTimeStr.isEmpty() ? QVariant(QDateTime::currentDateTimeUtc().toString(
                                   Qt::ISODate))
                             : QVariant(endTimeStr));
    query.addBindValue(
        QJsonDocument(extraData).toJson(QJsonDocument::Compact));
    query.addBindValue(spanId);
  }

  if (!query.exec()) {
    qWarning() << "Failed to close span:" << spanId
               << query.lastError().text();
  }
}

// Returns per-day break statistics for the given date range [from, to].
//
// Two queries:
// 1. Break spans — counts and durations from the spans table
// 2. Postpone events — counts from the events table
//
// Results are merged by date. Days with no data are omitted.
QList<DailyBreakStats> BreakDatabase::queryDailyBreakStats(QDate from, QDate to) {
  QList<DailyBreakStats> results;
  auto err = ensureDb();
  if (err.type() != QSqlError::NoError) return results;

  // Query break counts, completion, and duration from spans
  QSqlQuery query(m_db);
  query.prepare(R"(
    SELECT
      date(started_at, 'localtime') AS day,
      SUM(CASE WHEN json_extract(data, '$.type') = 'small' THEN 1 ELSE 0 END),
      SUM(CASE WHEN json_extract(data, '$.type') = 'big' THEN 1 ELSE 0 END),
      SUM(CASE WHEN json_extract(data, '$.normal-exit') = 1 THEN 1 ELSE 0 END),
      SUM(MAX(0, strftime('%s', COALESCE(ended_at, CURRENT_TIMESTAMP)) - strftime('%s', started_at)))
    FROM spans
    WHERE type = 'break'
      AND (ended_at IS NOT NULL OR id = (SELECT MAX(id) FROM spans WHERE ended_at IS NULL))
      AND date(started_at, 'localtime') BETWEEN ? AND ?
    GROUP BY day
    ORDER BY day
  )");
  query.addBindValue(from.toString(Qt::ISODate));
  query.addBindValue(to.toString(Qt::ISODate));

  QMap<QDate, DailyBreakStats> statsMap;
  if (query.exec()) {
    while (query.next()) {
      DailyBreakStats stats;
      stats.date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
      stats.smallBreaks = query.value(1).toInt();
      stats.bigBreaks = query.value(2).toInt();
      stats.completedBreaks = query.value(3).toInt();
      stats.totalBreakSeconds = query.value(4).toInt();
      statsMap[stats.date] = stats;
    }
  }

  // Query postpone counts from events
  QSqlQuery postponeQuery(m_db);
  postponeQuery.prepare(R"(
    SELECT date(created_at, 'localtime') AS day, COUNT(*)
    FROM events
    WHERE type = 'postpone'
      AND date(created_at, 'localtime') BETWEEN ? AND ?
    GROUP BY day
  )");
  postponeQuery.addBindValue(from.toString(Qt::ISODate));
  postponeQuery.addBindValue(to.toString(Qt::ISODate));

  if (postponeQuery.exec()) {
    while (postponeQuery.next()) {
      QDate day = QDate::fromString(postponeQuery.value(0).toString(), Qt::ISODate);
      statsMap[day].date = day;
      statsMap[day].postponeCount = postponeQuery.value(1).toInt();
    }
  }

  for (auto it = statsMap.constBegin(); it != statsMap.constEnd(); ++it) {
    results.append(it.value());
  }
  return results;
}

// Distributes the seconds of a time span across calendar days.
// For example, a span from 23:50 to 00:10 contributes 600s to day 1 and 600s to day 2.
static void splitSpanIntoDays(const QDateTime& start, const QDateTime& end,
                              QMap<QDate, int>& daySeconds) {
  QDateTime cursor = start;
  while (cursor < end) {
    QDate curDay = cursor.date();
    QDateTime nextMidnight(curDay.addDays(1), QTime(0, 0, 0));
    QDateTime spanEnd = std::min(nextMidnight, end);
    int secs = cursor.secsTo(spanEnd);
    if (secs > 0) daySeconds[curDay] += secs;
    cursor = spanEnd;
  }
}

// Returns per-day usage statistics for the given date range [from, to].
//
// Computes two metrics per day:
//   - activeSeconds: time in Normal + Meeting spans
//   - totalSeconds:  time in Normal + Meeting + Break + Pause spans
//
// Sleep spans are simply excluded from the query. Each span is split across
// midnight boundaries using splitSpanIntoDays().
QList<DailyUsageStats> BreakDatabase::queryDailyUsageStats(QDate from, QDate to) {
  QList<DailyUsageStats> results;
  auto err = ensureDb();
  if (err.type() != QSqlError::NoError) return results;

  QSqlQuery query(m_db);
  query.prepare(R"(
    SELECT datetime(started_at, 'localtime'),
           datetime(COALESCE(ended_at, CURRENT_TIMESTAMP), 'localtime'),
           type
    FROM spans
    WHERE type IN ('normal', 'meeting', 'break', 'pause')
      AND (ended_at IS NOT NULL OR id = (SELECT MAX(id) FROM spans WHERE ended_at IS NULL))
      AND date(started_at, 'localtime') BETWEEN ? AND ?
  )");
  query.addBindValue(from.toString(Qt::ISODate));
  query.addBindValue(to.toString(Qt::ISODate));
  if (!query.exec()) return results;

  QMap<QDate, int> activeByDay, totalByDay;

  while (query.next()) {
    QDateTime start = QDateTime::fromString(query.value(0).toString(), Qt::ISODate);
    QDateTime end = QDateTime::fromString(query.value(1).toString(), Qt::ISODate);
    QString type = query.value(2).toString();
    if (!start.isValid() || !end.isValid() || start >= end) continue;

    bool isActive = (type == "normal" || type == "meeting");

    QMap<QDate, int> daySeconds;
    splitSpanIntoDays(start, end, daySeconds);
    for (auto it = daySeconds.constBegin(); it != daySeconds.constEnd(); ++it) {
      if (it.key() >= from && it.key() <= to) {
        totalByDay[it.key()] += it.value();
        if (isActive) activeByDay[it.key()] += it.value();
      }
    }
  }

  for (auto it = totalByDay.constBegin(); it != totalByDay.constEnd(); ++it) {
    results.append({it.key(), activeByDay.value(it.key(), 0), it.value()});
  }
  return results;
}

// Splits a span into per-day segments, preserving actual QDateTime start/end
// (clipped at midnight boundaries).
struct DaySpanSegment {
  QDate date;
  QDateTime start;
  QDateTime end;
};

static QList<DaySpanSegment> splitSpanByDay(const QDateTime& start,
                                            const QDateTime& end) {
  QList<DaySpanSegment> segments;
  QDateTime cursor = start;
  while (cursor < end) {
    QDate curDay = cursor.date();
    QDateTime nextMidnight(curDay.addDays(1), QTime(0, 0, 0));
    QDateTime spanEnd = std::min(nextMidnight, end);
    if (cursor < spanEnd) {
      segments.append({curDay, cursor, spanEnd});
    }
    cursor = spanEnd;
  }
  return segments;
}

// Returns per-day timeline data for the given date range [from, to].
//
// Queries spans (normal, meeting, break, pause, focus) and postpone events,
// then splits spans by day and groups everything into DayTimelineData.
QList<DayTimelineData> BreakDatabase::queryDailyTimelines(QDate from, QDate to) {
  QList<DayTimelineData> results;
  auto err = ensureDb();
  if (err.type() != QSqlError::NoError) return results;

  QMap<QDate, DayTimelineData> dayMap;

  // Query spans
  QSqlQuery query(m_db);
  query.prepare(R"(
    SELECT datetime(started_at, 'localtime'),
           datetime(COALESCE(ended_at, CURRENT_TIMESTAMP), 'localtime'),
           type,
           json_extract(data, '$.reason')
    FROM spans
    WHERE type IN ('normal', 'meeting', 'break', 'pause', 'focus')
      AND (ended_at IS NOT NULL OR id = (SELECT MAX(id) FROM spans WHERE ended_at IS NULL))
      AND date(started_at, 'localtime') BETWEEN ? AND ?
  )");
  query.addBindValue(from.toString(Qt::ISODate));
  query.addBindValue(to.toString(Qt::ISODate));

  if (query.exec()) {
    while (query.next()) {
      QDateTime start =
          QDateTime::fromString(query.value(0).toString(), Qt::ISODate);
      QDateTime end =
          QDateTime::fromString(query.value(1).toString(), Qt::ISODate);
      QString type = query.value(2).toString();
      QString reason = query.value(3).toString();
      if (!start.isValid() || !end.isValid() || start >= end) continue;

      for (const auto& seg : splitSpanByDay(start, end)) {
        if (seg.date < from || seg.date > to) continue;
        auto& day = dayMap[seg.date];
        day.date = seg.date;
        TimelineSpan span{seg.start, seg.end, type, reason};
        if (type == "focus") {
          day.focusSpans.append(span);
        } else {
          day.spans.append(span);
        }
      }
    }
  }

  // Query postpone events
  QSqlQuery postponeQuery(m_db);
  postponeQuery.prepare(R"(
    SELECT datetime(created_at, 'localtime')
    FROM events
    WHERE type = 'postpone'
      AND date(created_at, 'localtime') BETWEEN ? AND ?
  )");
  postponeQuery.addBindValue(from.toString(Qt::ISODate));
  postponeQuery.addBindValue(to.toString(Qt::ISODate));

  if (postponeQuery.exec()) {
    while (postponeQuery.next()) {
      QDateTime time =
          QDateTime::fromString(postponeQuery.value(0).toString(), Qt::ISODate);
      if (!time.isValid()) continue;
      QDate day = time.date();
      if (day < from || day > to) continue;
      dayMap[day].date = day;
      dayMap[day].events.append({time, "postpone"});
    }
  }

  // Sort spans within each day by start time
  for (auto& day : dayMap) {
    std::sort(day.spans.begin(), day.spans.end(),
              [](const TimelineSpan& a, const TimelineSpan& b) {
                return a.start < b.start;
              });
    std::sort(day.focusSpans.begin(), day.focusSpans.end(),
              [](const TimelineSpan& a, const TimelineSpan& b) {
                return a.start < b.start;
              });
    std::sort(day.events.begin(), day.events.end(),
              [](const TimelineEvent& a, const TimelineEvent& b) {
                return a.time < b.time;
              });
  }

  for (auto it = dayMap.constBegin(); it != dayMap.constEnd(); ++it) {
    results.append(it.value());
  }
  return results;
}

