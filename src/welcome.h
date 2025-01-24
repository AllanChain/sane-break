// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_WELCOME_H
#define SANE_WELCOME_H
#include <QDialog>
#include <QWidget>

class WelcomeWindow : public QDialog {
  Q_OBJECT

 public:
  WelcomeWindow(QWidget *parent = nullptr);
  ~WelcomeWindow();
};
#endif  // SANE_WELCOME_H
