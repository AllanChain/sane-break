// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTest>

#include "core/db.h"

// Helper to insert an event with a specific UTC timestamp
static void insertEvent(QSqlDatabase& db, const QString& timestamp,
                        const QString& type, const QString& data = "{}") {
  QSqlQuery query(db);
  query.prepare("INSERT INTO events (created_at, type, data) VALUES (?, ?, ?)");
  query.addBindValue(timestamp);
  query.addBindValue(type);
  query.addBindValue(data);
  QVERIFY(query.exec());
}

// Helper to insert a span with specific UTC timestamps
static void insertSpan(QSqlDatabase& db, const QString& type,
                       const QString& startedAt, const QString& endedAt = {},
                       const QString& data = "{}") {
  QSqlQuery query(db);
  if (endedAt.isEmpty()) {
    query.prepare(
        "INSERT INTO spans (type, started_at, data) VALUES (?, ?, ?)");
    query.addBindValue(type);
    query.addBindValue(startedAt);
    query.addBindValue(data);
  } else {
    query.prepare(
        "INSERT INTO spans (type, started_at, ended_at, data) VALUES (?, ?, ?, ?)");
    query.addBindValue(type);
    query.addBindValue(startedAt);
    query.addBindValue(endedAt);
    query.addBindValue(data);
  }
  QVERIFY(query.exec());
}

class TestDb : public QObject {
  Q_OBJECT
 private:
  QSqlDatabase sqlDb;
  BreakDatabase* db = nullptr;

 private slots:
  void init() {
    sqlDb = QSqlDatabase::addDatabase("QSQLITE", "test-db");
    sqlDb.setDatabaseName(":memory:");
    db = new BreakDatabase(sqlDb);
    // Force DB initialization by logging a dummy event
    db->logEvent("test::init");
  }
  void cleanup() {
    delete db;
    db = nullptr;
    sqlDb = QSqlDatabase();
    QSqlDatabase::removeDatabase("test-db");
  }

  void empty_stats() {
    auto stats = db->queryDailyBreakStats(QDate(2024, 1, 1), QDate(2024, 1, 7));
    QCOMPARE(stats.size(), 0);
  }

  void break_counts() {
    QDate day = QDate(2024, 6, 15);
    QString ts = day.toString(Qt::ISODate) + " 12:00:00";

    // 2 small breaks, 1 big break, 2 completed (normal-exit = true)
    insertSpan(sqlDb, "break", ts, ts,
               R"({"type":"small","normal-exit":true})");
    insertSpan(sqlDb, "break", ts, ts,
               R"({"type":"small","normal-exit":false})");
    insertSpan(sqlDb, "break", ts, ts,
               R"({"type":"big","normal-exit":true})");

    QDateTime utcNoon(day, QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();

    auto stats = db->queryDailyBreakStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].smallBreaks, 2);
    QCOMPARE(stats[0].bigBreaks, 1);
    QCOMPARE(stats[0].completedBreaks, 2);
  }

  void postpone_count() {
    QDate day = QDate(2024, 6, 15);
    QString ts = day.toString(Qt::ISODate) + " 12:00:00";

    insertEvent(sqlDb, ts, "postpone", R"({"seconds":300})");
    insertEvent(sqlDb, ts, "postpone", R"({"seconds":600})");

    QDateTime utcNoon(day, QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();

    auto stats = db->queryDailyBreakStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].postponeCount, 2);
  }

  void break_duration() {
    QDate day = QDate(2024, 6, 15);

    // Break 1: 30 seconds
    insertSpan(sqlDb, "break", "2024-06-15 12:00:00", "2024-06-15 12:00:30",
               R"({"type":"small","normal-exit":true})");
    // Break 2: 120 seconds
    insertSpan(sqlDb, "break", "2024-06-15 12:10:00", "2024-06-15 12:12:00",
               R"({"type":"big","normal-exit":true})");

    QDateTime utcNoon(day, QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();

    auto stats = db->queryDailyBreakStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].totalBreakSeconds, 150);
  }

  void multiple_days() {
    insertSpan(sqlDb, "break", "2024-06-15 12:00:00", "2024-06-15 12:00:30",
               R"({"type":"small","normal-exit":true})");
    insertSpan(sqlDb, "break", "2024-06-16 12:00:00", "2024-06-16 12:01:00",
               R"({"type":"big","normal-exit":true})");

    QDateTime utcDay1(QDate(2024, 6, 15), QTime(12, 0, 0), QTimeZone::UTC);
    QDateTime utcDay2(QDate(2024, 6, 16), QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDay1 = utcDay1.toLocalTime().date();
    QDate localDay2 = utcDay2.toLocalTime().date();

    auto stats = db->queryDailyBreakStats(localDay1, localDay2);
    QVERIFY(stats.size() >= 1);

    int totalSmall = 0, totalBig = 0;
    for (const auto& s : stats) {
      totalSmall += s.smallBreaks;
      totalBig += s.bigBreaks;
    }
    QCOMPARE(totalSmall, 1);
    QCOMPARE(totalBig, 1);
  }

  void usage_basic_active_time() {
    // Normal span: 12:00 to 12:30 = 30 minutes
    insertSpan(sqlDb, "normal", "2024-06-15 12:00:00", "2024-06-15 12:30:00");

    QDateTime utcNoon(QDate(2024, 6, 15), QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();

    auto stats = db->queryDailyUsageStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].activeSeconds, 30 * 60);
    QCOMPARE(stats[0].totalSeconds, 30 * 60);
  }

  void usage_meeting_active_time() {
    // Normal span: 30 min, Meeting span: 20 min
    insertSpan(sqlDb, "normal", "2024-06-15 12:00:00", "2024-06-15 12:30:00");
    insertSpan(sqlDb, "meeting", "2024-06-15 12:30:00", "2024-06-15 12:50:00",
               R"({"scheduledSeconds":1200,"reason":"standup"})");

    QDateTime utcNoon(QDate(2024, 6, 15), QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();

    auto stats = db->queryDailyUsageStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].activeSeconds, 50 * 60);  // normal + meeting
    QCOMPARE(stats[0].totalSeconds, 50 * 60);
  }

  void usage_total_includes_break_pause() {
    // Normal: 20 min, Break: 5 min, Pause: 10 min
    insertSpan(sqlDb, "normal", "2024-06-15 12:00:00", "2024-06-15 12:20:00");
    insertSpan(sqlDb, "break", "2024-06-15 12:20:00", "2024-06-15 12:25:00",
               R"({"type":"small","normal-exit":true})");
    insertSpan(sqlDb, "pause", "2024-06-15 12:25:00", "2024-06-15 12:35:00");

    QDateTime utcNoon(QDate(2024, 6, 15), QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();

    auto stats = db->queryDailyUsageStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].activeSeconds, 20 * 60);  // only normal
    QCOMPARE(stats[0].totalSeconds, 35 * 60);   // normal + break + pause
  }

  void usage_sleep_adjustment() {
    // Two normal spans around a sleep span:
    // Normal: 12:00-12:15, Sleep: 12:15-13:15, Normal: 13:15-13:30
    // Effective active time = 15 + 15 = 30 min
    insertSpan(sqlDb, "normal", "2024-06-15 12:00:00", "2024-06-15 12:15:00");
    insertSpan(sqlDb, "sleep", "2024-06-15 12:15:00", "2024-06-15 13:15:00");
    insertSpan(sqlDb, "normal", "2024-06-15 13:15:00", "2024-06-15 13:30:00");

    QDateTime utcNoon(QDate(2024, 6, 15), QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();

    auto stats = db->queryDailyUsageStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].activeSeconds, 30 * 60);
    QCOMPARE(stats[0].totalSeconds, 30 * 60);
  }

  void usage_cross_day_split() {
    // Normal span from 23:00 to 01:00 next day (2 hours), crossing midnight
    insertSpan(sqlDb, "normal", "2024-06-15 23:00:00", "2024-06-16 01:00:00");

    QDateTime utcStart(QDate(2024, 6, 15), QTime(23, 0, 0), QTimeZone::UTC);
    QDateTime utcEnd(QDate(2024, 6, 16), QTime(1, 0, 0), QTimeZone::UTC);
    QDate localDay1 = utcStart.toLocalTime().date();
    QDate localDay2 = utcEnd.toLocalTime().date();

    auto stats = db->queryDailyUsageStats(localDay1, localDay2);
    int totalActive = 0;
    for (const auto& s : stats) totalActive += s.activeSeconds;
    QCOMPARE(totalActive, 2 * 3600);
  }

  void usage_last_unclosed_span() {
    // The last unclosed span should be included (treated as ending now)
    insertSpan(sqlDb, "normal", "2024-06-15 12:00:00");

    QDateTime utcNoon(QDate(2024, 6, 15), QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcNoon.toLocalTime().date();
    QDate today = QDate::currentDate();

    auto stats = db->queryDailyUsageStats(localDate, today);
    QVERIFY(stats.size() >= 1);
    int totalActive = 0;
    for (const auto& s : stats) totalActive += s.activeSeconds;
    QVERIFY(totalActive > 0);
  }

  void date_range_filtering() {
    // Span outside the query range should not appear
    insertSpan(sqlDb, "break", "2024-06-10 12:00:00", "2024-06-10 12:00:30",
               R"({"type":"small","normal-exit":true})");
    insertSpan(sqlDb, "break", "2024-06-15 12:00:00", "2024-06-15 12:00:30",
               R"({"type":"small","normal-exit":true})");

    QDateTime utcDay(QDate(2024, 6, 15), QTime(12, 0, 0), QTimeZone::UTC);
    QDate localDate = utcDay.toLocalTime().date();

    auto stats = db->queryDailyBreakStats(localDate, localDate);
    QCOMPARE(stats.size(), 1);
    QCOMPARE(stats[0].smallBreaks, 1);
  }
};

QTEST_GUILESS_MAIN(TestDb)
#include "test-db.moc"
