// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "heads-up-window.h"

#include <qglobal.h>

#include <QFont>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QWidget>
#include <QWindow>
#include <algorithm>

HeadsUpWindow::HeadsUpWindow(int totalSeconds, QColor backgroundColor,
                             QColor flashColor, QColor textColor, QWidget* parent)
    : QWidget(parent),
      m_totalSeconds(std::max(1, totalSeconds)),
      m_textColor(textColor) {
  // Static fill color blends background toward the flash color, 75/25
  m_fillColor =
      QColor::fromRgbF(0.75 * backgroundColor.redF() + 0.25 * flashColor.redF(),
                       0.75 * backgroundColor.greenF() + 0.25 * flashColor.greenF(),
                       0.75 * backgroundColor.blueF() + 0.25 * flashColor.blueF(),
                       0.75 * backgroundColor.alphaF() + 0.25 * flashColor.alphaF());
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setAttribute(Qt::WA_MacAlwaysShowToolWindow);
  setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint);
#ifdef Q_OS_MACOS
  setWindowFlag(Qt::ToolTip);
#else
  setWindowFlag(Qt::Tool);
#endif
  setCursor(Qt::PointingHandCursor);
  setFixedSize(PILL_WIDTH, PILL_HEIGHT);

  m_progressAnim = new QPropertyAnimation(this, "progress");
  m_progressAnim->setStartValue(1.0);
  m_progressAnim->setEndValue(0.0);
  m_progressAnim->setDuration(m_totalSeconds * 1000);
  connect(this, &HeadsUpWindow::progressChanged, this,
          QOverload<>::of(&QWidget::update));
  m_progressAnim->start();
  setTime(totalSeconds);
}

void HeadsUpWindow::setTime(int remainingSeconds) {
  int clampedSeconds = std::clamp(remainingSeconds, 0, m_totalSeconds);
  m_progressAnim->setCurrentTime((m_totalSeconds - clampedSeconds) * 1000);
}

void HeadsUpWindow::initSize(QScreen* screen) {
  m_screen = screen;
  QRect geo = screen->availableGeometry();
  move(geo.x() + (geo.width() - PILL_WIDTH) / 2, geo.y() + 16);
  createWinId();
  if (windowHandle() && screen) windowHandle()->setScreen(screen);
}

void HeadsUpWindow::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QRectF rect(0, 0, width(), height());
  qreal radius = height() / 2.0;

  // Clip to pill shape
  QPainterPath pillPath;
  pillPath.addRoundedRect(rect, radius, radius);
  painter.setClipPath(pillPath);

  // Draw empty background (semi-transparent)
  QColor emptyColor = m_fillColor;
  emptyColor.setAlphaF(m_fillColor.alphaF() * 0.3);
  painter.fillRect(rect, emptyColor);

  // Draw filled portion (progress from left)
  if (m_progress > 0) {
    QRectF fillRect(0, 0, width() * m_progress, height());
    painter.fillRect(fillRect, m_fillColor);
  }

  painter.setClipping(false);

  // Draw title text
  painter.setPen(m_textColor);
  QFont titleFont = painter.font();
  titleFont.setPixelSize(16);
  titleFont.setBold(true);
  painter.setFont(titleFont);
  QRectF topRect(0, 8, width(), height() / 2);
  painter.drawText(topRect, Qt::AlignHCenter | Qt::AlignTop, tr("Break soon"));

  // Draw subtitle text
  QFont subtitleFont = painter.font();
  subtitleFont.setPixelSize(12);
  subtitleFont.setBold(false);
  painter.setFont(subtitleFont);
  QRectF bottomRect(0, 0, width(), height() - 8);
  painter.drawText(bottomRect, Qt::AlignHCenter | Qt::AlignBottom,
                   tr("Click to start"));
}

void HeadsUpWindow::mousePressEvent(QMouseEvent*) { emit clicked(); }
