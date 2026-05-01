// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QLockFile>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QThread>
#include <QTranslator>
#include <Qt>

#include "app.h"
#include "app/welcome.h"
#include "app/widgets/language-select.h"
#include "config.h"
#ifdef SANE_BREAK_CLI_CONTROL_ENABLED
#include "core/cli.h"
#include "core/command-ipc.h"
#endif
#include "core/preferences.h"

#ifdef Q_OS_LINUX
#include "lib/linux/system-check.h"
#endif

bool waitForTray() {
  for (int attempt = 0; attempt < 5; attempt++) {
    if (QSystemTrayIcon::isSystemTrayAvailable()) return true;
    QThread::sleep(1);
  }
  qWarning() << "System tray not available.";
  return false;
}

#ifdef SANE_BREAK_CLI_CONTROL_ENABLED
int runCliCommand(int argc, char* argv[], const QStringList& arguments) {
  QCoreApplication a(argc, argv);

  CliCommandResult result = sendCliCommand(arguments);
  QTextStream stream(result.ok ? stdout : stderr);
  stream << result.message << Qt::endl;
  return result.ok ? 0 : 1;
}
#endif

int main(int argc, char* argv[]) {
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QCoreApplication::setApplicationVersion(PROJECT_VERSION);
  QSettings::setDefaultFormat(QSettings::IniFormat);

  QStringList rawArguments;
  for (int i = 1; i < argc; i++) {
    rawArguments.append(QString::fromLocal8Bit(argv[i]));
  }

#ifdef SANE_BREAK_CLI_CONTROL_ENABLED
  if (!shouldLaunchGuiForArguments(rawArguments)) {
    return runCliCommand(argc, argv, rawArguments);
  }
#endif

  QApplication a(argc, argv);
  a.setApplicationDisplayName("Sane Break");

  QCommandLineParser parser;
  parser.setApplicationDescription("Sane Break");
  parser.process(a);
  if (!parser.positionalArguments().isEmpty()) {
    QTextStream(stderr) << QCoreApplication::tr("Unexpected GUI argument: %1")
                               .arg(parser.positionalArguments().first())
                        << Qt::endl;
    return 1;
  }

  if (waitForTray()) a.setQuitOnLastWindowClosed(false);

  SanePreferences* preferences = SanePreferences::createDefault();

#ifdef WITH_TRANSLATIONS
  LanguageSelect::setLanguage(preferences->language->get());
#endif

  // Use lock file to avoid starting multiple instances of app
  const QString lockFilePath =
      QDir(QStandardPaths::writableLocation(QStandardPaths::RuntimeLocation))
#ifndef NDEBUG
          .filePath("sane-break-debug.lock");
#else
          .filePath("sane-break.lock");
#endif
  QLockFile lock(lockFilePath);
  lock.setStaleLockTime(0);
  if (!lock.tryLock() && lock.error() == QLockFile::LockFailedError) {
    QMessageBox msgBox;
    msgBox.setText(QCoreApplication::tr("Another instance of Sane Break is running."));
    msgBox.setInformativeText(QCoreApplication::tr(
        "Please quit the old instance before starting a new one. "
        "If the previous instance has already exited, you can start a new "
        "one anyway."));
    msgBox.setIcon(QMessageBox::Icon::Question);
    msgBox.addButton(QCoreApplication::tr("Quit"), QMessageBox::RejectRole);
    QPushButton* startBtn =
        msgBox.addButton(QCoreApplication::tr("Start Anyway"), QMessageBox::AcceptRole);
    msgBox.exec();
    if (msgBox.clickedButton() != startBtn) return 1;
    lock.removeStaleLockFile();
    if (!lock.tryLock()) {
      QMessageBox::critical(
          nullptr, "Sane Break",
          QCoreApplication::tr(
              "Could not start because another instance is still running. "
              "Please manually delete \"%1\" and try again.")
              .arg(lockFilePath));
      return 1;
    }
  }

#ifdef Q_OS_LINUX
  QDir appPath = a.applicationDirPath();
  appPath.cdUp();
  appPath.cd(CMAKE_INSTALL_LIBDIR);
  a.addLibraryPath(appPath.filePath("sane-break"));
  LinuxSystemSupport::check();
#endif  // Q_OS_LINUX

  QFontDatabase::addApplicationFont(":/fonts/bootstrap-icons.ttf");

  if (!QFile::exists(preferences->settings->fileName())) {
    WelcomeWindow* welcome = new WelcomeWindow(preferences);
    if (welcome->exec() == QDialog::Rejected) {
      return 0;
    } else {
      preferences->shownWelcome->set(true);
    }
    welcome->deleteLater();
  }

  SaneBreakApp* app = SaneBreakApp::create(preferences, &a);
#ifdef SANE_BREAK_CLI_CONTROL_ENABLED
  CliCommandServer commandServer(app);
  QString commandServerError;
  if (!commandServer.start(&commandServerError)) {
    qWarning().noquote() << QCoreApplication::tr("Could not start command server: %1")
                                .arg(commandServerError);
  }
#endif

  app->start();
  return a.exec();
}
