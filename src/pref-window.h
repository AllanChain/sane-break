// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_WINDOW_H
#define SANE_PREFERENCES_WINDOW_H

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QSlider>

class SteppedSlider : public QSlider {
  Q_OBJECT

 public:
  SteppedSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

 private:
  int calculateValueFromPosition(const QPoint &pos) const;
};

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  PreferenceWindow(QWidget *parent = nullptr);
  void loadSettings();
  void saveSettings();

 private:
  void closeEvent(QCloseEvent *event);
  SteppedSlider *smallBreakEverySlider;
  SteppedSlider *smallBreakForSlider;
  SteppedSlider *bigBreakAfterSlider;
  SteppedSlider *bigBreakForSlider;
  SteppedSlider *flashForSlider;
  SteppedSlider *pauseOnIdleSlider;
  QCheckBox *pauseOnBatteryCheck;
  QComboBox *bellSoundSelect;
};
#endif  // SANE_PREFERENCES_WINDOW_H
