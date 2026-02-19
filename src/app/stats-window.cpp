// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stats-window.h"

#include <QDate>
#include <QEvent>
#include <QIcon>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QString>
#include <QWidget>
#include <Qt>
#include <algorithm>

#include "core/db.h"
#include "ui_stats-window.h"
#include "widgets/timeline-view.h"

static QString formatDuration(int totalSeconds) {
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  if (hours > 0)
    return StatsWindow::tr("%1h %2m").arg(hours).arg(minutes);
  return StatsWindow::tr("%1m").arg(minutes);
}

StatsWindow::StatsWindow(BreakDatabase* db, QWidget* parent)
    : QWidget(parent), ui(new Ui::StatsWindow), m_db(db) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowIcon(QIcon(":/images/icon.png"));
  refreshData();
}

bool StatsWindow::event(QEvent* event) {
  if (event->type() == QEvent::WindowActivate) refreshData();
  return QWidget::event(event);
}

void StatsWindow::refreshData() {
  QDate today = QDate::currentDate();
  QDate weekAgo = today.addDays(-6);
  auto breakStats = m_db->queryDailyBreakStats(weekAgo, today);
  auto usageStats = m_db->queryDailyUsageStats(weekAgo, today);
  auto timelines = m_db->queryDailyTimelines(weekAgo, today);

  populateSummaryCards(breakStats, usageStats);
  populateDailyBreakdown(timelines, breakStats);
}

StatsWindow::~StatsWindow() { delete ui; }

void StatsWindow::populateSummaryCards(const QList<DailyBreakStats>& allStats,
                                       const QList<DailyUsageStats>& usageStats) {
  QDate today = QDate::currentDate();

  DailyBreakStats todayStats;
  todayStats.date = today;
  DailyBreakStats weekStats;

  for (const auto& day : allStats) {
    weekStats.smallBreaks += day.smallBreaks;
    weekStats.bigBreaks += day.bigBreaks;
    weekStats.completedBreaks += day.completedBreaks;
    weekStats.totalBreakSeconds += day.totalBreakSeconds;
    weekStats.postponeCount += day.postponeCount;
    if (day.date == today) {
      todayStats = day;
    }
  }

  int todayTotal = todayStats.smallBreaks + todayStats.bigBreaks;
  ui->todayBreaksLabel->setArgs({todayTotal, todayStats.completedBreaks});
  ui->todayTimeLabel->setArgs({formatDuration(todayStats.totalBreakSeconds)});
  ui->todayPostponeLabel->setArgs({todayStats.postponeCount});

  int weekTotal = weekStats.smallBreaks + weekStats.bigBreaks;
  ui->weekBreaksLabel->setArgs({weekTotal, weekStats.completedBreaks});
  ui->weekTimeLabel->setArgs({formatDuration(weekStats.totalBreakSeconds)});
  ui->weekPostponeLabel->setArgs({weekStats.postponeCount});

  // Usage stats
  DailyUsageStats todayUsage;
  DailyUsageStats weekUsage;
  for (const auto& day : usageStats) {
    weekUsage.activeSeconds += day.activeSeconds;
    weekUsage.totalSeconds += day.totalSeconds;
    if (day.date == today) {
      todayUsage = day;
    }
  }

  ui->todayActiveLabel->setArgs({formatDuration(todayUsage.activeSeconds)});
  ui->todayTotalLabel->setArgs({formatDuration(todayUsage.totalSeconds)});
  ui->weekActiveLabel->setArgs({formatDuration(weekUsage.activeSeconds)});
  ui->weekTotalLabel->setArgs({formatDuration(weekUsage.totalSeconds)});
}

void StatsWindow::populateDailyBreakdown(
    const QList<DayTimelineData>& timelines,
    const QList<DailyBreakStats>& allStats) {
  // Build lookup map
  QMap<QDate, DailyBreakStats> statsMap;
  for (const auto& s : allStats) statsMap[s.date] = s;

  // Compute shared time range across all days
  int rangeStart = 86400;  // will find min
  int rangeEnd = 0;        // will find max
  for (const auto& day : timelines) {
    for (const auto& span : day.spans) {
      int startSec = span.start.time().msecsSinceStartOfDay() / 1000;
      int endSec = span.end.time().msecsSinceStartOfDay() / 1000;
      if (endSec == 0 && span.end.date() > span.start.date()) endSec = 86400;
      rangeStart = std::min(rangeStart, startSec);
      rangeEnd = std::max(rangeEnd, endSec);
    }
    for (const auto& span : day.focusSpans) {
      int startSec = span.start.time().msecsSinceStartOfDay() / 1000;
      int endSec = span.end.time().msecsSinceStartOfDay() / 1000;
      if (endSec == 0 && span.end.date() > span.start.date()) endSec = 86400;
      rangeStart = std::min(rangeStart, startSec);
      rangeEnd = std::max(rangeEnd, endSec);
    }
  }

  // Fallback or round to hour boundaries
  if (rangeStart >= rangeEnd) {
    rangeStart = 8 * 3600;   // 08:00
    rangeEnd = 18 * 3600;    // 18:00
  } else {
    rangeStart = (rangeStart / 3600) * 3600;          // round down to hour
    rangeEnd = ((rangeEnd + 3599) / 3600) * 3600;     // round up to hour
    rangeEnd = std::min(rangeEnd, 86400);
  }

  ui->timelineView->populate(timelines, statsMap, rangeStart, rangeEnd);
}

