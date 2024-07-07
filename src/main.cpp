#ifdef __linux
#include <LayerShellQt/Shell>
#endif
#include <QApplication>
#include <QFile>
#include <QSettings>

#include "app.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QSettings::setDefaultFormat(QSettings::IniFormat);

  QFile styleSheet(":/src/style.css");

  QApplication a(argc, argv);
  if (styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
    a.setStyleSheet(styleSheet.readAll());
  SaneBreakApp *app = new SaneBreakApp();
  app->start();
  a.connect(app, &SaneBreakApp::quit, &a, &QCoreApplication::quit,
            Qt::QueuedConnection);

  return a.exec();
}
