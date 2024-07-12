#include "winmanager.h"

#include <qglobal.h>

#include <utility>

#ifdef Q_OS_LINUX
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#elif defined Q_OS_MACOS
#include "macos_window.h"
#endif
#include <QApplication>
#include <QList>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "idle/system.h"
#include "window.h"

BreakWindowManager::BreakWindowManager() : QObject() {
  countdownTimer = new QTimer(this);
  countdownTimer->setInterval(1000);
  idleTimer = SystemIdleTime::createIdleTimer();
  connect(countdownTimer, &QTimer::timeout, this, &BreakWindowManager::tick);
  connect(idleTimer, &SystemIdleTime::idleStart, this,
          &BreakWindowManager::onIdleStart);
  connect(idleTimer, &SystemIdleTime::idleEnd, this,
          &BreakWindowManager::onIdleEnd);
#ifdef Q_OS_LINUX
  LayerShellQt::Shell::useLayerShell();
#endif
}
BreakWindowManager::~BreakWindowManager() {};

void BreakWindowManager::createWindows() {
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : std::as_const(screens)) {
    BreakWindow *w = new BreakWindow();
    windows.append(w);
    w->initSize(screen);
    w->show();
    w->hide();
#ifdef Q_OS_LINUX
    if (auto window = LayerShellQt::Window::get(w->windowHandle())) {
      using namespace LayerShellQt;
      window->setCloseOnDismissed(true);
      window->setLayer(Window::LayerOverlay);
      window->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
      window->setAnchors(Window::AnchorTop);
    }
#elif defined Q_OS_MACOS
    macSetAllWorkspaces(w->windowHandle());
#endif
    w->show();
  }
}

void BreakWindowManager::show(int breakTime) {
  remainingTime = breakTime;
  totalTime = breakTime;
  isIdle = true;
  isForceBreak = false;
  createWindows();

  QTimer::singleShot(2 * 1000, this,
                     [=]() {  // Go fullscreen when idle for 2 sec
                       if (isIdle)
                         for (auto w : std::as_const(windows))
                           w->setFullScreen();
                     });
  QTimer::singleShot(30 * 1000, this, [=]() {  // Force break
    isForceBreak = true;
    for (auto w : std::as_const(windows)) w->setFullScreen();
  });

  for (auto w : std::as_const(windows)) w->start(breakTime);
  countdownTimer->start();
  idleTimer->startWatching();
}

void BreakWindowManager::close() {
  countdownTimer->stop();
  for (auto w : std::as_const(windows)) {
    w->close();
    w->deleteLater();
  }
  windows.clear();
  idleTimer->stopWatching();
  emit timeout();
}

void BreakWindowManager::tick() {
  bool shouldCountDown = isForceBreak || isIdle;
  if (!shouldCountDown) {
    for (auto w : std::as_const(windows)) w->setTime(remainingTime);
  } else {
    remainingTime--;
    for (auto w : std::as_const(windows)) w->setTime(remainingTime);
  }
  if (remainingTime <= 0) return close();
}

void BreakWindowManager::onIdleStart() {
  if (isForceBreak || remainingTime <= 0) return;
  for (auto w : std::as_const(windows)) {
    w->setFullScreen();
  }
  isIdle = true;
}

void BreakWindowManager::onIdleEnd() {
  if (isForceBreak || remainingTime <= 0) return;
  for (auto w : std::as_const(windows)) {
    w->resizeToNormal();
  }
  isIdle = false;
  remainingTime = totalTime;
}
