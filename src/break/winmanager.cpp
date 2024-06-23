#include "winmanager.h"

#include <KIdleTime>
#include <LayerShellQt/Shell>
#include <LayerShellQt/Window>
#include <QApplication>
#include <QList>
#include <QObject>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include "window.h"

#define TOTAL_TIME 10000
#define FRAME_RATE 25

BreakWindowManager::BreakWindowManager() : QObject() {}
BreakWindowManager::~BreakWindowManager(){};

void BreakWindowManager::createWindows() {
  QList<QScreen *> screens = QApplication::screens();

  for (QScreen *screen : screens) {
    BreakWindow *w = new BreakWindow();
    windows.append(w);
    QRect geometry = screen->geometry();
    geometry.setSize(QSize(300, 100));
    w->setGeometry(geometry);
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
}

void BreakWindowManager::show() {
  remainingTime = TOTAL_TIME;
  createWindows();
  countdownTimer = new QTimer(this);
  countdownTimer->setInterval(1000 / FRAME_RATE);

  connect(countdownTimer, &QTimer::timeout, this, &BreakWindowManager::tick);
  connect(KIdleTime::instance(), &KIdleTime::timeoutReached, this,
          &BreakWindowManager::onIdleStart);
  connect(KIdleTime::instance(), &KIdleTime::resumingFromIdle, this,
          &BreakWindowManager::onIdleEnd);
  QTimer::singleShot(2 * 1000, [=]() {  // Go fullscreen when idle for 2 sec
    if (isIdle)
      for (auto w : windows) w->setFullScreen();
  });
  QTimer::singleShot(30 * 1000, [=]() {  // Force break
    isForceBreak = true;
    for (auto w : windows) w->setFullScreen();
  });

  countdownTimer->start();
  KIdleTime::instance()->catchNextResumeEvent();
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
  // Listen to next idle end
  KIdleTime::instance()->catchNextResumeEvent();
}

void BreakWindowManager::onIdleEnd() {
  if (isForceBreak || remainingTime <= 0) return;
  for (auto w : windows) {
    w->resizeToNormal();
  }
  isIdle = false;
  remainingTime = TOTAL_TIME;
  // Listen to next idle start
  KIdleTime::instance()->addIdleTimeout(2000);
}
