#include "winmanager.h"

#ifdef __linux
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#endif
#include <QApplication>
#include <QList>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "idle/system.h"
#include "window.h"

#define TOTAL_TIME 10000
#define FRAME_RATE 25

BreakWindowManager::BreakWindowManager() : QObject() {
  countdownTimer = new QTimer(this);
  countdownTimer->setInterval(1000 / FRAME_RATE);
  idleTimer = SystemIdleTime::createIdleTimer();
  connect(countdownTimer, &QTimer::timeout, this, &BreakWindowManager::tick);
  connect(idleTimer, &SystemIdleTime::idleStart, this,
          &BreakWindowManager::onIdleStart);
  connect(idleTimer, &SystemIdleTime::idleEnd, this,
          &BreakWindowManager::onIdleEnd);
}
BreakWindowManager::~BreakWindowManager(){};

void BreakWindowManager::createWindows() {
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : screens) {
    BreakWindow *w = new BreakWindow();
    windows.append(w);
    w->setGeometry(screen->geometry());
    w->resizeToNormal();
    w->show();
    w->hide();
#ifdef __linux
    if (auto window = LayerShellQt::Window::get(w->windowHandle())) {
      using namespace LayerShellQt;
      window->setCloseOnDismissed(true);
      window->setLayer(Window::LayerOverlay);
      window->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
      window->setAnchors(Window::AnchorTop);
    }
#endif
    w->show();
  }
}

void BreakWindowManager::show() {
  remainingTime = TOTAL_TIME;
  isIdle = true;
  isForceBreak = false;
  createWindows();

  QTimer::singleShot(2 * 1000, [=]() {  // Go fullscreen when idle for 2 sec
    if (isIdle)
      for (auto w : windows) w->setFullScreen();
  });
  QTimer::singleShot(30 * 1000, [=]() {  // Force break
    isForceBreak = true;
    for (auto w : windows) w->setFullScreen();
  });

  countdownTimer->start();
  idleTimer->startWatching();
}

void BreakWindowManager::tick() {
  remainingTime -= countdownTimer->interval();
  bool shouldCountDown = isForceBreak || isIdle;
  if (!shouldCountDown) {
    if (remainingTime < TOTAL_TIME - 500) {
      remainingTime = TOTAL_TIME;
    }
  }
  if (remainingTime <= 0) {
    countdownTimer->stop();
    for (auto w : windows) {
      w->close();
      w->deleteLater();
    }
    windows.clear();
    idleTimer->stopWatching();
    emit timeout();
    return;
  }
  for (auto w : windows) w->tick(remainingTime);
}

void BreakWindowManager::onIdleStart() {
  if (isForceBreak || remainingTime <= 0) return;
  for (auto w : windows) {
    w->setFullScreen();
  }
  isIdle = true;
}

void BreakWindowManager::onIdleEnd() {
  if (isForceBreak || remainingTime <= 0) return;
  for (auto w : windows) {
    w->resizeToNormal();
  }
  isIdle = false;
  remainingTime = TOTAL_TIME;
}
