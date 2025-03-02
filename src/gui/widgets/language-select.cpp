// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "language-select.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QListIterator>
#include <QString>
#include <QStyleOptionSlider>
#include <QTranslator>
#include <QWidget>
#include <QtContainerFwd>

#include "preferences.h"

LanguageSelect::LanguageSelect(QWidget *parent) : QComboBox(parent) {
  addItem(tr("Default"), "");
  QDir dir(":/i18n");
  QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
  QListIterator<QString> i(fileNames);
  QTranslator translator;
  while (i.hasNext()) {
    QFileInfo fileInfo(i.next());
    QString language = fileInfo.baseName();
    if (translator.load(language, ":/i18n"))
      addItem(translator.translate("English", "current language"), language);
  }
  connect(this, &QComboBox::currentIndexChanged, this,
          &LanguageSelect::onLanguageSelect);
}

void LanguageSelect::onLanguageSelect() {
  QString language = currentData().toString();
  SanePreferences::language->set(language);
  QTranslator *translator = new QTranslator();
  if (language == "") {
    if (translator->load(QLocale::system(), "sane-break", "_", ":/i18n"))
      qApp->installTranslator(translator);
  } else {
    if (translator->load(language, ":/i18n")) {
      qApp->installTranslator(translator);
    }
  }
}
