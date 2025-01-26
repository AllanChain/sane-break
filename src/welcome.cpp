// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "welcome.h"

#include <qglobal.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>

#ifdef Q_OS_LINUX
#include "linux/system-check.h"
#endif

WelcomeWindow::WelcomeWindow(QWidget *parent) : QDialog(parent) {
  setWindowIcon(QIcon(":/images/icon_tray-color.png"));
  setWindowTitle("Welcome to Sane Break");
  setFixedWidth(400);
  setContentsMargins(10, 20, 10, 10);

  QVBoxLayout *layout = new QVBoxLayout();
  setLayout(layout);
  layout->setContentsMargins(12, 0, 12, 0);

  QLabel *icon = new QLabel(this);
  icon->setPixmap(QPixmap(":/images/icon-256.png"));
  icon->setMargin(12);
  icon->setScaledContents(true);
  icon->setMaximumWidth(140);
  icon->setMaximumHeight(140);
  layout->addWidget(icon);
  layout->setAlignment(icon, Qt::AlignHCenter);

  QLabel *welcome = new QLabel(this);
  welcome->setText(
      "<h3 align=center>Welcome to Sane Break!</h3>"
      "<p>Sane Break is a cross-platform break reminder designed to help "
      "you take meaningful breaks without disrupting your workflow. "
      "Sane Break will stay in the system tray and remind you to take "
      "breaks at regular intervals. More details are available at <a "
      "href=\"https://github.com/AllanChain/sane-break/\">GitHub</a>.</p>");
  welcome->setOpenExternalLinks(true);
  welcome->setWordWrap(true);
  welcome->setAlignment(Qt::AlignTop | Qt::AlignJustify);
  layout->addWidget(welcome);

  bool hasWarningOrError = false;
#ifdef Q_OS_LINUX
  QString errorText = "";
  for (auto &error : LinuxSystemSupport::errors()) {
    errorText += QString("<li>%1</li>").arg(error);
  }
  if (errorText.length() > 0) {
    hasWarningOrError = true;
    QLabel *errorLabel = new QLabel();
    errorLabel->setWordWrap(true);
    errorLabel->setStyleSheet("color: #ef4444");
    errorLabel->setText(QString("<ul>%1</ul>").arg(errorText));
    layout->addWidget(errorLabel);
  }
  QString warningText = "";
  for (auto &warning : LinuxSystemSupport::warnings()) {
    warningText += QString("<li>%1</li>").arg(warning);
  }
  if (warningText.length() > 0) {
    hasWarningOrError = true;
    QLabel *warningLabel = new QLabel();
    warningLabel->setWordWrap(true);
    warningLabel->setStyleSheet("color: #eab308");
    warningLabel->setText(QString("<ul>%1</ul>").arg(warningText));
    layout->addWidget(warningLabel);
  }
#endif
  layout->addSpacing(20);
  QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
  if (hasWarningOrError) {
    buttonBox->addButton("Ignore", QDialogButtonBox::AcceptRole);
    buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole)->setDefault(true);
  } else {
    buttonBox->addButton(QDialogButtonBox::Ok);
  }
  layout->addWidget(buttonBox);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

WelcomeWindow::~WelcomeWindow() {}
