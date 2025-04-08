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
      //: This is the name of the current language in the current language
      addItem(translator.translate("LanguageSelect", "English", "current language"),
              language);
    }
  }
  connect(this, &QComboBox::currentIndexChanged, this,
          &LanguageSelect::onLanguageSelect);
}

void LanguageSelect::setSelectedLanguage(QString language) {
  setCurrentIndex(findData(language));
}

void LanguageSelect::onLanguageSelect() {
  QString language = currentData().toString();
  emit languageChanged(language);
  setItemText(0, tr("Default"));
}
