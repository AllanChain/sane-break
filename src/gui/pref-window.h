// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_WINDOW_H
#define SANE_PREFERENCES_WINDOW_H
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QProcess>
#include <QPushButton>
#include <QSlider>
#include <QString>
#include <QWidget>

#include "sound-player.h"

namespace Ui {
class PrefWindow;
}

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  PreferenceWindow(QWidget *parent = nullptr);
  ~PreferenceWindow();
  void loadSettings();
  void saveSettings();

 private:
  Ui::PrefWindow *ui;
  QList<QPushButton *> tabButtons;
  QProcess *osaProcess;
  SoundPlayer *soundPlayer;
  void closeEvent(QCloseEvent *event);
  void setTab(int tabNum);
};
#endif  // SANE_PREFERENCES_WINDOW_H
