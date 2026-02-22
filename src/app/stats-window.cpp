// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "stats-window.h"

#include <QDate>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QPushButton>
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
  if (hours > 0) return StatsWindow::tr("%1h %2m").arg(hours).arg(minutes);
  return StatsWindow::tr("%1m").arg(minutes);
}

QDate StatsWindow::weekStartForDate(QDate date) {
  int dow = date.dayOfWeek();  // 1=Mon .. 7=Sun
  int first = QLocale().firstDayOfWeek();
  int diff = (dow - first + 7) % 7;
  return date.addDays(-diff);
}

StatsWindow::StatsWindow(BreakDatabase* db, QWidget* parent)
    : QWidget(parent), ui(new Ui::StatsWindow), m_db(db) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowIcon(QIcon(":/images/icon.png"));

  m_weekStart = weekStartForDate(QDate::currentDate());
  m_displayedDate = QDate::currentDate();

  connect(ui->prevWeekButton, &QPushButton::clicked, this,
          [this]() { navigateWeek(-1); });
  connect(ui->nextWeekButton, &QPushButton::clicked, this,
          [this]() { navigateWeek(1); });
  connect(ui->timelineView, &TimelineGraphicsView::dayHovered, this,
          &StatsWindow::updateDayDetail);

  // Build legend below the timeline (2 rows, 3 per row)
  auto* legendGrid = new QGridLayout();
  legendGrid->setHorizontalSpacing(12);
  legendGrid->setVerticalSpacing(4);
  auto items = timelineLegendItems();
  for (int i = 0; i < items.size(); i++) {
    int row = i / 3;
    int col = (i % 3) * 2;  // each item takes 2 columns (swatch + label)
    auto* swatch = new QFrame();
    swatch->setFixedSize(10, 10);
    swatch->setStyleSheet(QString("background-color: %1; border-radius: 2px;")
                              .arg(items[i].first.name()));
    auto* text = new QLabel(items[i].second);
    legendGrid->addWidget(swatch, row, col, Qt::AlignRight | Qt::AlignVCenter);
    legendGrid->addWidget(text, row, col + 1, Qt::AlignLeft | Qt::AlignVCenter);
  }
  ui->legendLayout->addStretch();
  ui->legendLayout->addLayout(legendGrid);
  ui->legendLayout->addStretch();

  refreshData();
}

bool StatsWindow::event(QEvent* event) {
  if (event->type() == QEvent::WindowActivate) refreshData();
  return QWidget::event(event);
}

void StatsWindow::refreshData() {
  QDate weekEnd = m_weekStart.addDays(6);
  auto breakStats = m_db->queryDailyBreakStats(m_weekStart, weekEnd);
  auto usageStats = m_db->queryDailyUsageStats(m_weekStart, weekEnd);
  auto timelines = m_db->queryDailyTimelines(m_weekStart, weekEnd);

  // Cache for hover lookup
  m_breakStatsMap.clear();
  for (const auto& s : breakStats) m_breakStatsMap[s.date] = s;
  m_usageStatsMap.clear();
  for (const auto& s : usageStats) m_usageStatsMap[s.date] = s;

  populateDailyBreakdown(timelines, breakStats);
  updateDayDetail(m_displayedDate);
  updateWeekLabel();

  // Disable next button when viewing the current week
  QDate currentWeekStart = weekStartForDate(QDate::currentDate());
  ui->nextWeekButton->setEnabled(m_weekStart < currentWeekStart);
}

StatsWindow::~StatsWindow() { delete ui; }

void StatsWindow::navigateWeek(int delta) {
  m_weekStart = m_weekStart.addDays(delta * 7);
  QDate today = QDate::currentDate();
  QDate weekEnd = m_weekStart.addDays(6);
  if (today >= m_weekStart && today <= weekEnd) {
    m_displayedDate = today;
  } else {
    m_displayedDate = weekEnd;
  }
  refreshData();
}

void StatsWindow::updateDayDetail(QDate date) {
  m_displayedDate = date;

  QLocale locale;
  ui->dayDetailGroup->setTitle(locale.toString(date, "dddd, MMM d"));

  DailyBreakStats stats = m_breakStatsMap.value(date);
  int totalBreaks = stats.smallBreaks + stats.bigBreaks;
  ui->dayBreaksLabel->setArgs({totalBreaks, stats.completedBreaks});
  ui->dayTimeLabel->setArgs({formatDuration(stats.totalBreakSeconds)});
  ui->dayPostponeLabel->setArgs({stats.postponeCount});

  DailyUsageStats usage = m_usageStatsMap.value(date);
  ui->dayActiveLabel->setArgs({formatDuration(usage.activeSeconds)});
  ui->dayTotalLabel->setArgs({formatDuration(usage.totalSeconds)});
}

void StatsWindow::updateWeekLabel() {
  QLocale locale;
  QDate weekEnd = m_weekStart.addDays(6);
  ui->weekLabel->setText(locale.toString(m_weekStart, "MMM d") + " – " +
                         locale.toString(weekEnd, "MMM d"));
}

void StatsWindow::populateDailyBreakdown(const QList<DayTimelineData>& timelines,
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
    rangeStart = 8 * 3600;  // 08:00
    rangeEnd = 18 * 3600;   // 18:00
  } else {
    rangeStart = (rangeStart / 3600) * 3600;       // round down to hour
    rangeEnd = ((rangeEnd + 3599) / 3600) * 3600;  // round up to hour
    rangeEnd = std::min(rangeEnd, 86400);
  }

  ui->timelineView->populate(timelines, rangeStart, rangeEnd, m_weekStart);
}
