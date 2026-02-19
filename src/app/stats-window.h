// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QEvent>
#include <QList>
#include <QWidget>

#include "core/db.h"

namespace Ui {
class StatsWindow;
}

class StatsWindow : public QWidget {
  Q_OBJECT

 public:
  StatsWindow(BreakDatabase* db, QWidget* parent = nullptr);
  ~StatsWindow();

 protected:
  bool event(QEvent* event) override;

 private:
  Ui::StatsWindow* ui;
  BreakDatabase* m_db;

  void refreshData();
  void populateSummaryCards(const QList<DailyBreakStats>& stats,
                            const QList<DailyUsageStats>& usageStats);
  void populateDailyBreakdown(const QList<DayTimelineData>& timelines,
                              const QList<DailyBreakStats>& breakStats);
};
