// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCoreApplication>
#include <QDialog>
#include <QFile>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <Qt>

#include "app.h"
#include "preferences.h"
#include "welcome.h"

#ifdef Q_OS_LINUX
#include "linux/system-check.h"
#endif

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QApplication a(argc, argv);

  QFile styleSheet(":/style.css");
  if (styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
    a.setStyleSheet(styleSheet.readAll());

  a.setApplicationDisplayName("Sane Break");
  if (QSystemTrayIcon::isSystemTrayAvailable()) a.setQuitOnLastWindowClosed(false);

  QTranslator translator;
  if (SanePreferences::language->get().length() > 0) {
    if (translator.load("sane-break_" + SanePreferences::language->get(), ":/i18n"))
      a.installTranslator(&translator);
  } else {
    if (translator.load(QLocale::system(), "sane-break", "_", ":/i18n"))
      a.installTranslator(&translator);
  }

#ifdef Q_OS_LINUX
  LinuxSystemSupport::check();
#endif  // Q_OS_LINUX

  if (!QFile::exists(getSettings().fileName())) {
    WelcomeWindow *welcome = new WelcomeWindow();
    if (welcome->exec() == QDialog::Rejected) {
      return 0;
    } else {
      SanePreferences::shownWelcome->set(true);
    }
    welcome->deleteLater();
  }

  SaneBreakApp *app = new SaneBreakApp();
  a.connect(app, &SaneBreakApp::quit, &a, &QCoreApplication::quit,
            Qt::QueuedConnection);

  app->start();
  return a.exec();
}
