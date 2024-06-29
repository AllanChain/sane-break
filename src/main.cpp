#ifdef __linux
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#endif
#include <QApplication>
#include <QSettings>

#include "app.h"

int main(int argc, char *argv[]) {
#ifdef __linux
  LayerShellQt::Shell::useLayerShell();
#endif
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
