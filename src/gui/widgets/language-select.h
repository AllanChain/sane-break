// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
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
