// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "language-select.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QListIterator>
#include <QString>
#include <QStyleOptionSlider>
#include <QTranslator>
#include <QWidget>
#include <QtContainerFwd>

#include "lib/preferences.h"

QTranslator *LanguageSelect::currentTranslator = nullptr;

void LanguageSelect::setLanguage(QString language) {
  QTranslator *translator = new QTranslator();
  if (language == "" &&
          translator->load(QLocale::system(), "sane-break", "_", ":/i18n") ||
      translator->load(language, ":/i18n")) {
    if (LanguageSelect::currentTranslator)
      qApp->removeTranslator(LanguageSelect::currentTranslator);
    qApp->installTranslator(translator);
    LanguageSelect::currentTranslator = translator;
  }
}

LanguageSelect::LanguageSelect(QWidget *parent) : QComboBox(parent) {
  addItem(tr("Default"), "");
  QDir dir(":/i18n");
  QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
  QListIterator<QString> i(fileNames);
  QTranslator translator;
  while (i.hasNext()) {
    QFileInfo fileInfo(i.next());
    QString language = fileInfo.baseName();
    if (language == "en")
      addItem("English", language);
    else if (translator.load(language, ":/i18n")) {
      addItem(translator.translate("LanguageSelect", "English", "current language"),
              language);
    }
  }
  setCurrentIndex(findData(SanePreferences::language->get()));
  connect(this, &QComboBox::currentIndexChanged, this,
          &LanguageSelect::onLanguageSelect);
}

void LanguageSelect::onLanguageSelect() {
  QString language = currentData().toString();
  emit languageChanged(language);
  setItemText(0, tr("Default"));
}
