// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColor>
#include <QCoreApplication>
#include <QDate>
#include <QEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QResizeEvent>
#include <QString>
#include <QWheelEvent>
#include <QWidget>

#include "core/db.h"

QList<QPair<QColor, QString>> timelineLegendItems();

class TimeAxisItem : public QGraphicsItem {
 public:
  TimeAxisItem(int startSecond, int endSecond, qreal baseWidth);
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

 private:
  static constexpr int kHeight = 20;
  int m_rangeStart;
  int m_rangeEnd;
  qreal m_baseWidth;
};

class DayTimelineItem : public QGraphicsItem {
 public:
  DayTimelineItem(qreal baseWidth);
  void setData(const DayTimelineData& data);
  void setTimeRange(int startSecond, int endSecond);
  void setTooltipText(const QString& text);
  QString tooltipAtSceneX(qreal sceneX) const;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget) override;

 private:
  static constexpr int kTrackHeight = 16;
  static constexpr int kFocusHeight = 4;
  DayTimelineData m_data;
  int m_rangeStart = 0;
  int m_rangeEnd = 86400;
  qreal m_baseWidth;
  QString m_tooltip;

  qreal timeToX(int secondOfDay) const;
  int xToTime(qreal x) const;
};

class TimelineGraphicsView : public QGraphicsView {
  Q_OBJECT
 public:
  explicit TimelineGraphicsView(QWidget* parent = nullptr);
  void populate(const QList<DayTimelineData>& timelines,
                const QMap<QDate, DailyBreakStats>& statsMap, int rangeStart,
                int rangeEnd);

 protected:
  void resizeEvent(QResizeEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  bool event(QEvent* event) override;
  bool viewportEvent(QEvent* event) override;
  bool eventFilter(QObject* obj, QEvent* event) override;

 private:
  static constexpr int kLabelWidth = 36;
  static constexpr int kRowSpacing = 4;
  static constexpr int kTimeAxisHeight = 20;
  static constexpr int kTrackHeight = 16;
  static constexpr qreal kSceneBaseWidth = 1000.0;
  static constexpr int kPixelPadding = 20;
  static constexpr qreal kMinZoom = 1.0;
  static constexpr qreal kMaxZoom = 10.0;

  qreal m_zoomFactor = 1.0;
  int m_contentHeight = 0;
  QWidget* m_labelArea = nullptr;
  QList<QPair<QString, DayTimelineItem*>> m_rows;  // label, item

  void applyZoom();
  void updateFixedHeight();
  qreal baseScale() const;
};
