// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "welcome.h"

#include <qglobal.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <Qt>

#include "ui_welcome.h"

WelcomeWindow::WelcomeWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::WelcomeDialog) {
  ui->setupUi(this);
  setWindowIcon(QIcon(":/images/icon_tray-color.png"));
  setWindowTitle("Welcome to Sane Break");
#ifdef Q_OS_LINUX
  if (QGuiApplication::platformName() != "wayland") {
    ui->warning->setHidden(true);
  } else {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString desktop = env.value("XDG_CURRENT_DESKTOP", "Unknown");
    if (desktop == "KDE") {
      ui->warning->setHidden(true);
    } else {
      setFixedHeight(height() + 50);
      ui->warning->setText(ui->warning->text().arg(desktop));
      ui->buttonBox->clear();
      ui->buttonBox->addButton("Ignore", QDialogButtonBox::AcceptRole);
      QPushButton *cancel =
          ui->buttonBox->addButton("Cancel", QDialogButtonBox::RejectRole);
      cancel->setDefault(true);
    }
  }
#else
  ui->warning->setHidden(true);
#endif
}

WelcomeWindow::~WelcomeWindow() { delete ui; }
