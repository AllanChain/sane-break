// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
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
#include "config.h"
#include "preferences.h"
#include "welcome.h"
#include "widgets/language-select.h"

#ifdef Q_OS_LINUX
#include "linux/system-check.h"
#endif

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QApplication a(argc, argv);

  a.setApplicationDisplayName("Sane Break");
  if (QSystemTrayIcon::isSystemTrayAvailable()) a.setQuitOnLastWindowClosed(false);

#ifdef WITH_TRANSLATIONS
  QTranslator translator;
  if (SanePreferences::language->get().length() > 0 &&
      translator.load(SanePreferences::language->get(), ":/i18n")) {
    a.installTranslator(&translator);
    LanguageSelect::currentTranslator = &translator;
  } else if (translator.load(QLocale::system(), "sane-break", "_", ":/i18n")) {
    a.installTranslator(&translator);
    LanguageSelect::currentTranslator = &translator;
  }
#endif

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
