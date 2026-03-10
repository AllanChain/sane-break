// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractAnimation>
#include <QColor>
#include <QPropertyAnimation>
#include <QScreen>
#include <QWidget>

class HeadsUpWindow : public QWidget {
  Q_OBJECT
  Q_PROPERTY(qreal progress MEMBER m_progress NOTIFY progressChanged)
  Q_PROPERTY(QColor flashColor MEMBER m_flashColor NOTIFY flashColorChanged)

 public:
  HeadsUpWindow(int totalSeconds, QColor bgColor, QColor highlightColor,
                QColor textColor, QWidget* parent = nullptr);
  void initSize(QScreen* screen);

 signals:
  void clicked();
  void progressChanged();
  void flashColorChanged();

 protected:
  void paintEvent(QPaintEvent*) override;
  void mousePressEvent(QMouseEvent*) override;

 private:
  qreal m_progress = 1.0;
  QColor m_bgColor;
  QColor m_flashColor;
  QColor m_textColor;
  QPropertyAnimation* m_progressAnim;
  QAbstractAnimation* m_flashAnim;

  static const int PILL_WIDTH = 280;
  static const int PILL_HEIGHT = 56;
};
