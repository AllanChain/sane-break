#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>

#include "app.h"

int main(int argc, char *argv[]) {
  LayerShellQt::Shell::useLayerShell();
  QCoreApplication::setOrganizationName("Allan Chain");
  QCoreApplication::setOrganizationDomain("allanchain.github.io");
  QCoreApplication::setApplicationName("Sane Break");

  QApplication a(argc, argv);
  SaneBreakApp *app = new SaneBreakApp();
  app->start();
  a.connect(app, &SaneBreakApp::quit, &a, &QCoreApplication::quit,
            Qt::QueuedConnection);

  return a.exec();
}
