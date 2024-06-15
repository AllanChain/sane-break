#include "mainwindow.h"

#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  LayerShellQt::Shell::useLayerShell();
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : screens) {
    MainWindow *w = new MainWindow();
    w->setGeometry(screen->availableGeometry());
    w->showFullScreen();
    w->hide();
    if (auto window = LayerShellQt::Window::get(w->windowHandle())) {
      using namespace LayerShellQt;
      window->setCloseOnDismissed(true);
      window->setLayer(Window::LayerOverlay);
      window->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
    }
    w->show();
  }

  return a.exec();
}
