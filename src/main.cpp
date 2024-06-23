#include <qcoreapplication.h>
#include <qobject.h>

#include <KIdleTime>
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "break/winmanager.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  LayerShellQt::Shell::useLayerShell();

  BreakWindowManager *breakManager = new BreakWindowManager();
  QObject::connect(breakManager, &BreakWindowManager::timeout, [=]() {
    QTimer::singleShot(20 * 1000, breakManager, &BreakWindowManager::show);
  });

  breakManager->show();
  return a.exec();
}
