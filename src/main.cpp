// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCoreApplication>
#include <QDialog>
#include <QFile>
#include <QSettings>
#include <Qt>

#include "app.h"
#include "preferences.h"
#include "welcome.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QSettings::setDefaultFormat(QSettings::IniFormat);

  QApplication a(argc, argv);

  QFile styleSheet(":/style.css");
  if (styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
    a.setStyleSheet(styleSheet.readAll());

  a.setApplicationDisplayName("Sane Break");
  a.setQuitOnLastWindowClosed(false);

  if (SanePreferences::shownWelcome->get() == false) {
    WelcomeWindow *welcome = new WelcomeWindow();
    welcome->show();
    if (welcome->exec() == QDialog::Rejected) {
      return 0;
    } else {
      SanePreferences::shownWelcome->set(true);
    }
  }

  SaneBreakApp *app = new SaneBreakApp();
  a.connect(app, &SaneBreakApp::quit, &a, &QCoreApplication::quit,
            Qt::QueuedConnection);

  app->start();
  return a.exec();
}
