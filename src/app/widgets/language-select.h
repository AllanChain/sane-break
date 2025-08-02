// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QComboBox>
#include <QEvent>
#include <QList>
#include <QString>
#include <QTranslator>
#include <QWidget>

class LanguageSelect : public QComboBox {
  Q_OBJECT
 public:
  static QList<QTranslator *> currentTranslators;
  LanguageSelect(QWidget *parent = nullptr);
  static void setLanguage(QString language);
  void setSelectedLanguage(QString language);

 signals:
  void languageChanged(QString language);

 private:
  void onLanguageSelect();
};
