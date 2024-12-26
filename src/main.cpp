// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QSettings>
#include <Qt>

#include "app.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QSettings::setDefaultFormat(QSettings::IniFormat);

  QApplication a(argc, argv);
  a.setApplicationDisplayName("Sane Break");
  SaneBreakApp *app = new SaneBreakApp();

  QFile styleSheet(":/style.css");
  if (styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
    a.setStyleSheet(styleSheet.readAll());

  a.setQuitOnLastWindowClosed(false);
  a.connect(app, &SaneBreakApp::quit, &a, &QCoreApplication::quit,
            Qt::QueuedConnection);

  app->start();
  return a.exec();
}
