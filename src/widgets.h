// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_WIDGETS_H
#define SANE_WIDGETS_H
#include <QMouseEvent>
#include <QSlider>

class SteppedSlider : public QSlider {
  Q_OBJECT

 public:
  SteppedSlider(QWidget *parent = nullptr);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

 private:
  int calculateValueFromPosition(const QPoint &pos) const;
};
#endif  // SANE_WIDGETS_H
