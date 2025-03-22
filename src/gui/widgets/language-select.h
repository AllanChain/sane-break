// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_WIDGETS_LANGUAGE_H
#define SANE_WIDGETS_LANGUAGE_H

#include <QComboBox>
#include <QEvent>
#include <QString>
#include <QTranslator>
#include <QWidget>

class LanguageSelect : public QComboBox {
  Q_OBJECT
 public:
  static QTranslator *currentTranslator;
  LanguageSelect(QWidget *parent = nullptr);
  static void setLanguage(QString language);

 signals:
  void languageChanged(QString language);

 private:
  void onLanguageSelect();
};

#endif  // SANE_WIDGETS_LANGUAGE_H
