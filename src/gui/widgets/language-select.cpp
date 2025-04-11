// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "language-select.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QList>
#include <QListIterator>
#include <QString>
#include <QStyleOptionSlider>
#include <QTranslator>
#include <QWidget>
#include <QtContainerFwd>

#include "config.h"

QList<QTranslator *> LanguageSelect::currentTranslators = {};

void LanguageSelect::setLanguage(QString language) {
  QTranslator *translator = new QTranslator();
  if (language == "" && !translator->load(QLocale::system(), "", "", ":/i18n") ||
      !translator->load(language, ":/i18n")) {
    if (language == "en") return;
    // Fall back to English
    return setLanguage("en");
  }
  if (!currentTranslators.isEmpty()) {
    const QList<QTranslator *> translatorsToRemove = currentTranslators;
    for (QTranslator *translatorToRemove : translatorsToRemove) {
      qApp->removeTranslator(translatorToRemove);
    }
    currentTranslators.empty();
  }
  qApp->installTranslator(translator);
  currentTranslators.append(translator);

  QLocale::setDefault(QLocale(language));

  QTranslator *qtBaseTranslater = new QTranslator();
#ifdef BUNDLE_QT_TRANSLATIONS
  if (language == "" &&
          !qtBaseTranslater->load(QLocale::system(), "", "", ":/i18n-qtbase") ||
      !qtBaseTranslater->load(language, ":/i18n-qtbase")) {
    qDebug() << "Failed to set Qt base translator" << language;
    return;
  }
#else
  QString translatorPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
  if (language == "" &&
          !qtBaseTranslater->load(QLocale::system(), "qtbase", "_", translatorPath) ||
      !qtBaseTranslater->load("qtbase_" + language, translatorPath)) {
    qDebug() << "Failed to set Qt base translator" << language;
    return;
  }
#endif
  qApp->installTranslator(qtBaseTranslater);
  currentTranslators.append(qtBaseTranslater);
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
