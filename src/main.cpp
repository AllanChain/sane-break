#include <qcoreapplication.h>
#include <qobject.h>

#include <KIdleTime>
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "break/scheduler.h"
#include "tray.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  LayerShellQt::Shell::useLayerShell();
  QCoreApplication::setOrganizationName("Allan Chain");
  QCoreApplication::setOrganizationDomain("allanchain.github.io");
  QCoreApplication::setApplicationName("Sane Break");

  BreakScheduler *scheduler = new BreakScheduler();
  TrayIcon *tray = new TrayIcon();
  scheduler->start();

  return a.exec();
}
