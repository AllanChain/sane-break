// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2025-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

struct TimelineSpan {
  QDateTime start;
  QDateTime end;
  QString type;    // "normal", "break", "pause", "meeting", "focus"
  QString reason;  // optional, from span JSON data (focus/meeting)
};

struct TimelineEvent {
  QDateTime time;
  QString type;  // "postpone"
};

struct DayTimelineData {
  QDate date;
  QList<TimelineSpan> spans;       // normal, break, pause, meeting
  QList<TimelineSpan> focusSpans;  // focus (rendered as overlay)
  QList<TimelineEvent> events;     // postpone events
};

struct DailyBreakStats {
  QDate date;
  int smallBreaks = 0;
  int bigBreaks = 0;
  int completedBreaks = 0;
  int totalBreakSeconds = 0;
  int postponeCount = 0;
};

struct DailyUsageStats {
  QDate date;
  int activeSeconds = 0;  // Normal + Meeting
  int totalSeconds = 0;   // Normal + Meeting + Break + Pause (excludes sleep)
};

class BreakDatabase : public QObject {
  Q_OBJECT
 public:
  BreakDatabase(QSqlDatabase, QObject* parent = nullptr);
  ~BreakDatabase();
  static QString dbPath();
  QSqlError logEvent(const QString& eventType, const QJsonObject& eventData = {});
  int openSpan(const QString& type, const QJsonObject& data = {},
               const QDateTime& startTime = {});
  void closeSpan(int spanId, const QJsonObject& extraData = {},
                 const QDateTime& endTime = {});
  QList<DailyBreakStats> queryDailyBreakStats(QDate from, QDate to);
  QList<DailyUsageStats> queryDailyUsageStats(QDate from, QDate to);
  QList<DayTimelineData> queryDailyTimelines(QDate from, QDate to);

 protected:
  QSqlError ensureDb();
  QSqlDatabase m_db;
};
