// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "widgets/timeline-view.h"

#include <QColor>
#include <QCoreApplication>
#include <QDate>
#include <QEvent>
#include <QFontMetrics>
#include <QFrame>
#include <QGestureEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHelpEvent>
#include <QList>
#include <QLocale>
#include <QMap>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QResizeEvent>
#include <QScrollBar>
#include <QString>
#include <QStyleOptionGraphicsItem>
#include <QToolTip>
#include <QTransform>
#include <QWheelEvent>
#include <QWidget>
#include <Qt>
#include <algorithm>

#include "core/db.h"

static QColor colorForType(const QString& type) {
  if (type == "normal") return QColor("#4ade80");    // green-400
  if (type == "break") return QColor("#facc15");     // yellow-400
  if (type == "pause") return QColor("#9ca3af");     // gray-400
  if (type == "meeting") return QColor("#60a5fa");   // blue-400
  if (type == "focus") return QColor("#a78bfa");     // violet-400
  if (type == "postpone") return QColor("#f87171");  // red-400
  return QColor(128, 128, 128, 80);                  // semi-transparent gray
}

static QString labelForType(const QString& type) {
  if (type == "normal")
    return QCoreApplication::translate("DayTimelineWidget", "Active");
  if (type == "break") return QCoreApplication::translate("DayTimelineWidget", "Break");
  if (type == "pause") return QCoreApplication::translate("DayTimelineWidget", "Pause");
  if (type == "meeting")
    return QCoreApplication::translate("DayTimelineWidget", "Meeting");
  if (type == "focus") return QCoreApplication::translate("DayTimelineWidget", "Focus");
  if (type == "postpone")
    return QCoreApplication::translate("DayTimelineWidget", "Postpone");
  return type;
}

static const QList<QString> kLegendTypes = {"normal",  "break", "pause",
                                            "meeting", "focus", "postpone"};

QList<QPair<QColor, QString>> timelineLegendItems() {
  QList<QPair<QColor, QString>> items;
  for (const auto& type : kLegendTypes)
    items.append({colorForType(type), labelForType(type)});
  return items;
}

// --- TimeAxisItem ---

TimeAxisItem::TimeAxisItem(int startSecond, int endSecond, qreal baseWidth)
    : m_rangeStart(startSecond), m_rangeEnd(endSecond), m_baseWidth(baseWidth) {}

QRectF TimeAxisItem::boundingRect() const { return QRectF(0, 0, m_baseWidth, kHeight); }

void TimeAxisItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/,
                         QWidget* widget) {
  painter->setRenderHint(QPainter::Antialiasing, false);

  QColor textColor =
      widget ? widget->palette().color(QPalette::WindowText) : QColor("#6b7280");

  // The painter's transform includes the view scale; extract it so we can
  // counter-scale text and adapt tick density to actual pixel width.
  qreal hScale = painter->transform().m11();

  auto timeToX = [&](int secondOfDay) -> qreal {
    if (m_rangeEnd <= m_rangeStart) return 0;
    return static_cast<qreal>(secondOfDay - m_rangeStart) /
           (m_rangeEnd - m_rangeStart) * m_baseWidth;
  };

  QFont font = painter->font();
  font.setPixelSize(10);
  painter->setFont(font);

  int totalHours = (m_rangeEnd - m_rangeStart) / 3600;
  // Adapt tick density based on effective pixel width per hour
  qreal pixelsPerHour = (m_baseWidth / totalHours) * hScale;
  int stepHours;
  if (pixelsPerHour >= 60)
    stepHours = 1;
  else if (pixelsPerHour >= 30)
    stepHours = 2;
  else
    stepHours = 4;

  QPen tickPen(textColor);
  tickPen.setCosmetic(true);
  painter->setPen(tickPen);

  QFontMetrics fm(font);
  int startHour = m_rangeStart / 3600;
  int endHour = m_rangeEnd / 3600;
  for (int h = startHour; h <= endHour; h += stepHours) {
    int sec = h * 3600;
    qreal x = timeToX(sec);
    QString label = QString("%1:00").arg(h, 2, 10, QChar('0'));

    // Tick mark (cosmetic pen keeps constant pixel width)
    painter->drawLine(QPointF(x, 0), QPointF(x, 4));

    // Counter-scale text so it doesn't stretch with zoom
    int textWidth = fm.horizontalAdvance(label);
    painter->save();
    painter->translate(x, 0);
    painter->scale(1.0 / hScale, 1.0);
    painter->drawText(QPointF(-textWidth / 2.0, kHeight - 4), label);
    painter->restore();
  }
}

// --- DayTimelineItem ---

DayTimelineItem::DayTimelineItem(qreal baseWidth) : m_baseWidth(baseWidth) {
  setAcceptHoverEvents(true);
}

void DayTimelineItem::setData(const DayTimelineData& data) { m_data = data; }

void DayTimelineItem::setTimeRange(int startSecond, int endSecond) {
  m_rangeStart = startSecond;
  m_rangeEnd = endSecond;
}

void DayTimelineItem::setTooltipText(const QString& text) { m_tooltip = text; }

QRectF DayTimelineItem::boundingRect() const {
  return QRectF(0, 0, m_baseWidth, kTrackHeight);
}

qreal DayTimelineItem::timeToX(int secondOfDay) const {
  if (m_rangeEnd <= m_rangeStart) return 0;
  return static_cast<qreal>(secondOfDay - m_rangeStart) / (m_rangeEnd - m_rangeStart) *
         m_baseWidth;
}

int DayTimelineItem::xToTime(qreal x) const {
  if (m_baseWidth <= 0 || m_rangeEnd <= m_rangeStart) return m_rangeStart;
  return m_rangeStart + static_cast<int>(x / m_baseWidth * (m_rangeEnd - m_rangeStart));
}

static int spanSecond(const TimelineSpan& span, bool start) {
  int sec = start ? span.start.time().msecsSinceStartOfDay() / 1000
                  : span.end.time().msecsSinceStartOfDay() / 1000;
  if (!start && sec == 0 && span.end.date() > span.start.date()) sec = 86400;
  return sec;
}

static QString spanTooltip(const QString& label, const TimelineSpan& span) {
  QString tip = label + "\n" + span.start.time().toString("H:mm") + " - " +
                span.end.time().toString("H:mm");
  if (!span.reason.isEmpty()) tip += "\n" + span.reason;
  return tip;
}

QString DayTimelineItem::tooltipAtSceneX(qreal sceneX) const {
  // Convert scene X to local item X
  qreal localX = sceneX - pos().x();
  int sec = xToTime(localX);

  // Check focus spans first (they overlay on top)
  for (const auto& span : m_data.focusSpans) {
    if (sec >= spanSecond(span, true) && sec < spanSecond(span, false))
      return spanTooltip(labelForType(span.type), span);
  }

  // Check base spans
  for (const auto& span : m_data.spans) {
    if (sec >= spanSecond(span, true) && sec < spanSecond(span, false))
      return spanTooltip(labelForType(span.type), span);
  }

  return m_tooltip;
}

void DayTimelineItem::paint(QPainter* painter,
                            const QStyleOptionGraphicsItem* /*option*/,
                            QWidget* /*widget*/) {
  painter->setRenderHint(QPainter::Antialiasing, false);
  qreal w = m_baseWidth;
  int h = kTrackHeight;

  // Semi-transparent gray background — adapts to both light and dark themes
  painter->fillRect(QRectF(0, 0, w, h), QColor(128, 128, 128, 40));

  // Base track spans
  for (const auto& span : m_data.spans) {
    int startSec = span.start.time().msecsSinceStartOfDay() / 1000;
    int endSec = span.end.time().msecsSinceStartOfDay() / 1000;
    if (endSec == 0 && span.end.date() > span.start.date()) endSec = 86400;

    qreal x1 = timeToX(startSec);
    qreal x2 = timeToX(endSec);
    qreal spanWidth = std::max(x2 - x1, 1.0);
    painter->fillRect(QRectF(x1, 0, spanWidth, h), colorForType(span.type));
  }

  // Focus overlay (bottom 4px)
  for (const auto& span : m_data.focusSpans) {
    int startSec = span.start.time().msecsSinceStartOfDay() / 1000;
    int endSec = span.end.time().msecsSinceStartOfDay() / 1000;
    if (endSec == 0 && span.end.date() > span.start.date()) endSec = 86400;

    qreal x1 = timeToX(startSec);
    qreal x2 = timeToX(endSec);
    qreal spanWidth = std::max(x2 - x1, 1.0);
    painter->fillRect(QRectF(x1, h - kFocusHeight, spanWidth, kFocusHeight),
                      colorForType("focus"));
  }

  // Postpone tick marks
  QPen tickPen(colorForType("postpone"));
  tickPen.setWidthF(2);
  tickPen.setCosmetic(true);
  painter->setPen(tickPen);
  for (const auto& event : m_data.events) {
    int sec = event.time.time().msecsSinceStartOfDay() / 1000;
    qreal x = timeToX(sec);
    painter->drawLine(QPointF(x, 0), QPointF(x, h));
  }
}

// --- TimelineGraphicsView ---

TimelineGraphicsView::TimelineGraphicsView(QWidget* parent) : QGraphicsView(parent) {
  setViewportUpdateMode(FullViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFrameShape(QFrame::NoFrame);
  setRenderHint(QPainter::Antialiasing, false);
  setDragMode(ScrollHandDrag);
  setViewportMargins(kLabelWidth, 0, 0, 0);
  grabGesture(Qt::PinchGesture);

  m_labelArea = new QWidget(this);
  m_labelArea->setAttribute(Qt::WA_TransparentForMouseEvents);
  m_labelArea->installEventFilter(this);
  m_labelArea->raise();

  viewport()->setMouseTracking(true);
}

qreal TimelineGraphicsView::baseScale() const {
  qreal viewWidth = viewport()->width() - 2 * kPixelPadding;
  if (viewWidth <= 0) return 1.0;
  return viewWidth / kSceneBaseWidth;
}

void TimelineGraphicsView::applyZoom() {
  qreal scale = baseScale() * m_zoomFactor;
  setTransform(QTransform().scale(scale, 1.0));
  // Recompute scene rect so padding stays constant in pixels
  if (scene()) {
    qreal pad = kPixelPadding / scale;
    scene()->setSceneRect(-pad, 0, kSceneBaseWidth + 2 * pad, m_contentHeight);
  }
  updateFixedHeight();
  m_labelArea->update();
}

void TimelineGraphicsView::updateFixedHeight() {
  setFixedHeight(m_contentHeight + horizontalScrollBar()->sizeHint().height());
}

void TimelineGraphicsView::populate(const QList<DayTimelineData>& timelines,
                                    const QMap<QDate, DailyBreakStats>& statsMap,
                                    int rangeStart, int rangeEnd, QDate weekStart) {
  m_rows.clear();
  m_zoomFactor = 1.0;
  m_lastHoveredDate = QDate();

  auto* sc = new QGraphicsScene(this);

  qreal yPos = 0;

  // Time axis
  auto* axisItem = new TimeAxisItem(rangeStart, rangeEnd, kSceneBaseWidth);
  axisItem->setPos(0, yPos);
  sc->addItem(axisItem);
  yPos += kTimeAxisHeight + kRowSpacing;

  // Day rows
  QLocale locale;
  for (int i = 0; i < 7; i++) {
    QDate day = weekStart.addDays(i);

    // Find matching timeline
    DayTimelineData timeline;
    timeline.date = day;
    for (const auto& t : timelines) {
      if (t.date == day) {
        timeline = t;
        break;
      }
    }

    auto* item = new DayTimelineItem(kSceneBaseWidth);
    item->setTimeRange(rangeStart, rangeEnd);
    item->setData(timeline);

    // Build tooltip
    DailyBreakStats stats = statsMap.value(day);
    int totalBreaks = stats.smallBreaks + stats.bigBreaks;
    QString tooltip =
        locale.toString(day, QLocale::ShortFormat) + "\n" +
        QCoreApplication::translate("StatsWindow", "Breaks: %1 (%2 completed)")
            .arg(totalBreaks)
            .arg(stats.completedBreaks) +
        "\n" +
        QCoreApplication::translate("StatsWindow", "Break time: %1")
            .arg(QCoreApplication::translate("StatsWindow", "%1h %2m")
                     .arg(stats.totalBreakSeconds / 3600)
                     .arg((stats.totalBreakSeconds % 3600) / 60)) +
        "\n" +
        QCoreApplication::translate("StatsWindow", "Postpones: %1")
            .arg(stats.postponeCount);
    item->setTooltipText(tooltip);

    item->setPos(0, yPos);
    sc->addItem(item);

    QString label = locale.toString(day, "ddd");
    m_rows.append({label, day, item});
    yPos += kTrackHeight + kRowSpacing;
  }

  sc->setSceneRect(0, 0, kSceneBaseWidth, yPos);
  setScene(sc);
  updateSceneBackground();

  m_contentHeight = static_cast<int>(yPos);
  applyZoom();
}

void TimelineGraphicsView::updateSceneBackground() {
  if (scene()) scene()->setBackgroundBrush(palette().color(QPalette::Window));
}

void TimelineGraphicsView::resizeEvent(QResizeEvent* event) {
  QGraphicsView::resizeEvent(event);
  m_labelArea->setFixedSize(kLabelWidth, height());
  applyZoom();
}

void TimelineGraphicsView::changeEvent(QEvent* event) {
  QGraphicsView::changeEvent(event);
  if (event->type() == QEvent::PaletteChange) {
    updateSceneBackground();
    viewport()->update();
  }
}

void TimelineGraphicsView::wheelEvent(QWheelEvent* event) {
  // Mouse wheel (no phase) zooms; trackpad two-finger scroll (has phase) pans
  if (event->phase() == Qt::NoScrollPhase) {
    QPointF scenePos = mapToScene(event->position().toPoint());
    qreal factor = 1.0 + event->angleDelta().y() / 600.0;
    m_zoomFactor = std::clamp(m_zoomFactor * factor, kMinZoom, kMaxZoom);

    applyZoom();

    QPointF newViewportPos = mapFromScene(scenePos);
    QPointF delta = newViewportPos - event->position();
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                    static_cast<int>(delta.x()));
    event->accept();
  } else {
    QGraphicsView::wheelEvent(event);
  }
}

void TimelineGraphicsView::mouseMoveEvent(QMouseEvent* event) {
  QGraphicsView::mouseMoveEvent(event);
  QPointF scenePos = mapToScene(event->pos());
  for (const auto& [label, date, item] : m_rows) {
    QRectF itemRect = item->sceneBoundingRect();
    if (scenePos.y() >= itemRect.top() && scenePos.y() < itemRect.bottom()) {
      if (date != m_lastHoveredDate) {
        m_lastHoveredDate = date;
        emit dayHovered(date);
      }
      return;
    }
  }
}

bool TimelineGraphicsView::event(QEvent* event) {
  if (event->type() == QEvent::NativeGesture) {
    auto* gesture = static_cast<QNativeGestureEvent*>(event);
    if (gesture->gestureType() == Qt::ZoomNativeGesture) {
      QPointF scenePos = mapToScene(gesture->position().toPoint());
      qreal factor = 1.0 + gesture->value();
      m_zoomFactor = std::clamp(m_zoomFactor * factor, kMinZoom, kMaxZoom);

      applyZoom();

      QPointF newViewportPos = mapFromScene(scenePos);
      QPointF delta = newViewportPos - gesture->position();
      horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                      static_cast<int>(delta.x()));
      return true;
    }
  }
  if (event->type() == QEvent::Gesture) {
    auto* ge = static_cast<QGestureEvent*>(event);
    if (auto* pinch = static_cast<QPinchGesture*>(ge->gesture(Qt::PinchGesture))) {
      if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged) {
        m_zoomFactor =
            std::clamp(m_zoomFactor * pinch->scaleFactor(), kMinZoom, kMaxZoom);
        applyZoom();
      }
      return true;
    }
  }
  return QGraphicsView::event(event);
}

bool TimelineGraphicsView::viewportEvent(QEvent* event) {
  if (event->type() == QEvent::ToolTip) {
    auto* helpEvent = static_cast<QHelpEvent*>(event);
    QPointF scenePos = mapToScene(helpEvent->pos());

    // Check if in label area (viewport coords)
    if (helpEvent->pos().x() < kLabelWidth) {
      QToolTip::hideText();
      return true;
    }

    // Find which row item we're hovering
    for (const auto& [label, date, item] : m_rows) {
      QRectF itemRect = item->sceneBoundingRect();
      if (scenePos.y() >= itemRect.top() && scenePos.y() < itemRect.bottom()) {
        QString tip = item->tooltipAtSceneX(scenePos.x());
        if (!tip.isEmpty()) {
          QToolTip::showText(helpEvent->globalPos(), tip, viewport());
        } else {
          QToolTip::hideText();
        }
        return true;
      }
    }

    QToolTip::hideText();
    return true;
  }
  return QGraphicsView::viewportEvent(event);
}

bool TimelineGraphicsView::eventFilter(QObject* obj, QEvent* event) {
  if (obj == m_labelArea && event->type() == QEvent::Paint) {
    QPainter painter(m_labelArea);
    painter.fillRect(m_labelArea->rect(), palette().color(QPalette::Window));
    painter.setPen(palette().color(QPalette::WindowText));

    for (const auto& [label, date, item] : m_rows) {
      QPointF viewPos = mapFromScene(item->pos());
      qreal y = viewPos.y();
      QRectF labelRect(0, y, kLabelWidth - 4, kTrackHeight);
      painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }
    return true;
  }
  return QGraphicsView::eventFilter(obj, event);
}
