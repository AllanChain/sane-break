// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "welcome.h"

#include <qglobal.h>

#include <QApplication>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QLabel>
#include <QListIterator>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtContainerFwd>

#include "preferences.h"

#ifdef Q_OS_LINUX
#include "linux/system-check.h"
#endif

WelcomeWindow::WelcomeWindow(QWidget *parent) : QDialog(parent) {
  setWindowIcon(QIcon(":/images/icon.png"));
  setFixedWidth(400);
  setContentsMargins(10, 20, 10, 10);

  QVBoxLayout *layout = new QVBoxLayout(this);
  setLayout(layout);
  layout->setContentsMargins(12, 0, 12, 0);

  QLabel *icon = new QLabel(this);
  icon->setPixmap(QPixmap(":/images/icon.png"));
  icon->setMargin(12);
  icon->setScaledContents(true);
  icon->setMaximumWidth(140);
  icon->setMaximumHeight(140);
  layout->addWidget(icon);
  layout->setAlignment(icon, Qt::AlignHCenter);

  welcome = new QLabel(this);
  welcome->setWordWrap(true);
  welcome->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  layout->addWidget(welcome);

  QHBoxLayout *hlayout = new QHBoxLayout(this);
  languageLabel = new QLabel(this);
  hlayout->addWidget(languageLabel);
  languageSelect = new QComboBox(this);
  languageSelect->addItem(tr("System Language"), "");
  QDir dir(":/i18n");
  QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
  QListIterator<QString> i(fileNames);
  QTranslator translator;
  while (i.hasNext()) {
    QString fileName = i.next();
    QFileInfo fileInfo(fileName);
    if (translator.load(fileName))
      languageSelect->addItem(translator.translate("English", "current language"),
                              fileInfo.baseName().split("_").last());
  }
  hlayout->addWidget(languageSelect);
  layout->addItem(hlayout);

  bool hasError = false;
  errorLabel = new QLabel(this);
  errorLabel->setWordWrap(true);
  errorLabel->setStyleSheet("color: #ef4444");

  bool hasWarning = false;
  warningLabel = new QLabel(this);
  warningLabel->setWordWrap(true);
  warningLabel->setStyleSheet("color: #eab308");

#ifdef Q_OS_LINUX
  hasError = LinuxSystemSupport::errors().length();
  hasWarning = LinuxSystemSupport::warnings().length();
#endif
  if (hasError) layout->addWidget(errorLabel);
  if (hasWarning) layout->addWidget(warningLabel);

  layout->addSpacing(20);
  QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
  docButton = buttonBox->addButton(tr("Read More"), QDialogButtonBox::HelpRole);
  if (hasError) {
    ignoreButton = buttonBox->addButton(tr("Ignore"), QDialogButtonBox::AcceptRole);
    cancelButton = buttonBox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
    cancelButton->setDefault(true);
  } else {
    okButton = buttonBox->addButton(tr("OK"), QDialogButtonBox::AcceptRole);
  }
  layout->addWidget(buttonBox);
  updateText();
  connect(languageSelect, &QComboBox::currentIndexChanged, this,
          &WelcomeWindow::onLanguageSelect);
  connect(docButton, &QPushButton::clicked, this, [=]() {
    QDesktopServices::openUrl(QUrl("https://github.com/AllanChain/sane-break/#readme"));
  });
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

WelcomeWindow::~WelcomeWindow() {}

void WelcomeWindow::updateText() {
  setWindowTitle(tr("Welcome to Sane Break"));
  welcome->setText(tr(
      "<h3 align=center>Welcome to Sane Break!</h3>"
      "<p>Sane Break is a cross-platform break reminder designed to help you take "
      "meaningful breaks without disrupting your workflow. Sane Break will stay in the "
      "system tray and remind you to take breaks at regular intervals. To quit, go to "
      "\"Postpone\" in the tray menu.</p>"));
  languageLabel->setText(tr("Language"));
  docButton->setText(tr("Read More"));
  if (ignoreButton) ignoreButton->setText(tr("Ignore"));
  if (cancelButton) cancelButton->setText(tr("Cancel"));
  if (okButton) okButton->setText(tr("OK"));
#ifdef Q_OS_LINUX
  QString errorText = "";
  for (auto &error : LinuxSystemSupport::errors()) {
    errorText += QString("<li>%1</li>").arg(error);
  }
  if (errorText.length() > 0) {
    errorLabel->setText(QString("<ul>%1</ul>").arg(errorText));
  }
  QString warningText = "";
  for (auto &warning : LinuxSystemSupport::warnings()) {
    warningText += QString("<li>%1</li>").arg(warning);
  }
  if (warningText.length() > 0) {
    warningLabel->setText(QString("<ul>%1</ul>").arg(warningText));
  }
#endif
  adjustSize();
}

void WelcomeWindow::onLanguageSelect() {
  QString language = languageSelect->currentData().toString();
  SanePreferences::language->set(language);
  QTranslator *translator = new QTranslator();
  if (language == "") {
    if (translator->load(QLocale::system(), "sane-break", "_", ":/i18n"))
      qApp->installTranslator(translator);
  } else {
    if (translator->load("sane-break_" + language, ":/i18n")) {
      qApp->installTranslator(translator);
    }
  }
}

void WelcomeWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange) {
    updateText();
  }
  QWidget::changeEvent(event);
}
