#include "mainwindow.h"

#include <KIdleTime>
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QScreen>
#include <QTimer>

void onBreak() {
  QList<MainWindow *> windows;
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : screens) {
    MainWindow *w = new MainWindow();
    windows.append(w);
    w->setGeometry(screen->geometry());
    w->resize(300, 100);
    w->show();
    w->hide();
    if (auto window = LayerShellQt::Window::get(w->windowHandle())) {
      using namespace LayerShellQt;
      window->setCloseOnDismissed(true);
      window->setLayer(Window::LayerOverlay);
      window->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
      window->setAnchors(Window::AnchorTop);
    }
    w->show();
  }

  QObject::connect(KIdleTime::instance(), &KIdleTime::resumingFromIdle, [=]() {
    KIdleTime::instance()->addIdleTimeout(1000);
    for (auto w : windows) {
      if (w->isVisible())
        w->onIdleEnd();
    }
  });
  QObject::connect(KIdleTime::instance(), &KIdleTime::timeoutReached,
                   [=](int id, int timeout) {
                     bool shouldCatchResume = false;
                     if (timeout == 1000) {
                       for (auto w : windows) {
                         if (w->isVisible()) {
                           w->onIdleStart();
                           shouldCatchResume = true;
                         }
                       }
                     }
                     if (shouldCatchResume)
                       KIdleTime::instance()->catchNextResumeEvent();
                   });
  KIdleTime::instance()->catchNextResumeEvent();
}

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  LayerShellQt::Shell::useLayerShell();
  QTimer *breakTimer = new QTimer();
  breakTimer->setInterval(20 * 1000);

  QObject::connect(breakTimer, &QTimer::timeout, onBreak);

  breakTimer->start();
  onBreak();
  return a.exec();
}
