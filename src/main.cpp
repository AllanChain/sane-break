#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QSettings>

#include "app.h"

int main(int argc, char *argv[]) {
  LayerShellQt::Shell::useLayerShell();
  QCoreApplication::setOrganizationName("SaneBreak");
  QCoreApplication::setApplicationName("SaneBreak");
  QSettings::setDefaultFormat(QSettings::IniFormat);

  QApplication a(argc, argv);
  SaneBreakApp *app = new SaneBreakApp();
  app->start();
  a.connect(app, &SaneBreakApp::quit, &a, &QCoreApplication::quit,
            Qt::QueuedConnection);

  return a.exec();
}
