#include "mainwindow.h"
#include <KIdleTime>

#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  LayerShellQt::Shell::useLayerShell();
  QList<MainWindow *> windows;
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : screens) {
    MainWindow *w = new MainWindow();
    windows.append(w);
    w->window()->setScreen(screen);
    w->show();
    w->hide();
    if (auto window = LayerShellQt::Window::get(w->windowHandle())) {
      using namespace LayerShellQt;
      window->setCloseOnDismissed(true);
      window->setLayer(Window::LayerOverlay);
      window->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
      window->setAnchors(Window::AnchorBottom);
    }
    w->show();
  }
  QObject::connect(KIdleTime::instance(), &KIdleTime::resumingFromIdle, [=]() {
    KIdleTime::instance()->addIdleTimeout(1000);
    for (auto w : windows) {
      w->onIdleEnd();
    }
  });
  QObject::connect(KIdleTime::instance(), &KIdleTime::timeoutReached,
                   [=](int id, int timeout) {
                     if (timeout == 1000) {
                       KIdleTime::instance()->catchNextResumeEvent();
                       for (auto w : windows) {
                         w->onIdleStart();
                       }
                     }
                   });
  KIdleTime::instance()->catchNextResumeEvent();

  return a.exec();
}
