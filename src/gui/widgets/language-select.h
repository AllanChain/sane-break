// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_WIDGETS_LANGUAGE_H
#define SANE_WIDGETS_LANGUAGE_H

#include <QComboBox>
#include <QWidget>

class LanguageSelect : public QComboBox {
  Q_OBJECT
 public:
  LanguageSelect(QWidget *parent = nullptr);

 private:
  void onLanguageSelect();
};

#endif  // SANE_WIDGETS_LANGUAGE_H
