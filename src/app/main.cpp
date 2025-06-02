// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <Qt>

#include "app.h"
#include "config.h"
#include "core/preferences.h"
#include "gui/welcome.h"
#include "gui/widgets/language-select.h"

#ifdef Q_OS_LINUX
#include "lib/linux/system-check.h"
#endif

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QApplication a(argc, argv);

  a.setApplicationDisplayName("Sane Break");
  if (QSystemTrayIcon::isSystemTrayAvailable()) a.setQuitOnLastWindowClosed(false);

  SanePreferences *preferences = SanePreferences::createDefault();

#ifdef WITH_TRANSLATIONS
  LanguageSelect::setLanguage(preferences->language->get());
#endif

#ifdef Q_OS_LINUX
  QDir appPath = a.applicationDirPath();
  appPath.cdUp();
  appPath.cd("lib");
  a.addLibraryPath(appPath.filePath("sane-break"));
  LinuxSystemSupport::check();
#endif  // Q_OS_LINUX

  if (!QFile::exists(preferences->settings->fileName())) {
    WelcomeWindow *welcome = new WelcomeWindow(preferences);
    if (welcome->exec() == QDialog::Rejected) {
      return 0;
    } else {
      preferences->shownWelcome->set(true);
    }
    welcome->deleteLater();
  }

  SaneBreakApp *app = SaneBreakApp::create(preferences);

  app->start();
  return a.exec();
}
