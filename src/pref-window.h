// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_H
#define SANE_PREFERENCES_H

#include <QLabel>
#include <QMainWindow>
#include <QSlider>

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  PreferenceWindow(QWidget *parent = nullptr);
  void loadSettings();
  void saveSettings();

 private:
  void closeEvent(QCloseEvent *event);
  QSlider *smallBreakEverySlider;
  QSlider *smallBreakForSlider;
  QSlider *bigBreakAfterSlider;
  QSlider *bigBreakForSlider;
};
#endif  // SANE_PREFERENCES_H
