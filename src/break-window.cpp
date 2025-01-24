// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "break-window.h"

#include <qglobal.h>

#include <QApplication>
#include <QColor>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QRect>
#include <QScreen>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>
#include <Qt>
#include <cmath>

#include "config.h"

#ifdef Q_OS_LINUX
#include "linux/system-check.h"
#endif  // Q_OS_LINUX
#ifdef LayerShellQt_FOUND
#include <LayerShellQt/window.h>
#elif defined Q_OS_MACOS
#include "macos/workspace.h"
#endif

BreakWindow::BreakWindow(BreakType type, QWidget *parent) : QMainWindow(parent) {
#ifdef Q_OS_LINUX
  // Positioning windows on Wayland is nearly impossible without layer shell protol.
  // In Wayland workaround mode, the main window is transparent and takes up all
  // available space on the screen (but not the system panels), and the main widget
  // changes size and covers the space.
  waylandWorkaround =
      QGuiApplication::platformName() == "wayland" && !LinuxSystemSupport::layerShell;
#endif
  setAttribute(Qt::WA_TranslucentBackground);      // transparent window
  setAttribute(Qt::WA_ShowWithoutActivating);      // avoid gaining keyboard focus
  setAttribute(Qt::WA_LayoutOnEntireRect);         // ignore safe zone on macOS
  setAttribute(Qt::WA_TransparentForMouseEvents);  // mouse can click through
  setWindowFlags(Qt::ToolTip | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint);
  setProperty("isFullScreen", false);

  mainWidget = new QWidget(this);
  if (!waylandWorkaround) setCentralWidget(mainWidget);
  mainWidget->setAttribute(Qt::WA_LayoutOnEntireRect);
  mainWidget->setContentsMargins(0, 0, 0, 0);

  QVBoxLayout *layout = new QVBoxLayout(mainWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  QProgressBar *progressBar = new QProgressBar();
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);

  QVBoxLayout *textLayout = new QVBoxLayout();
  textLayout->setAlignment(Qt::AlignCenter);
  layout->addLayout(textLayout);

  QLabel *breakLabel = new QLabel(
      QString("Time for a %1 break").arg(type == BreakType::BIG ? "big" : "small"));
  breakLabel->setObjectName("breakLabel");
  textLayout->addWidget(breakLabel);

  countdownLabel = new QLabel(this);
  countdownLabel->setObjectName("countdownLabel");
  countdownLabel->setAlignment(Qt::AlignCenter);
  countdownLabel->setVisible(false);
  textLayout->addWidget(countdownLabel);

  progressAnim = new QPropertyAnimation(progressBar, "value");
  progressAnim->setStartValue(100);
  progressAnim->setEndValue(0);

  bgAnim = new QPropertyAnimation(this, "color");
  if (type == BreakType::BIG)
    bgAnim->setStartValue(QColor(180, 142, 173, 100));
  else
    bgAnim->setStartValue(QColor(235, 203, 139, 100));
  bgAnim->setEndValue(QColor(46, 52, 64, 255));
  bgAnim->setDuration(500);
  bgAnim->setLoopCount(-1);
}

BreakWindow::~BreakWindow() {}

void BreakWindow::colorChanged() {
  setStyleSheet(QString("BreakWindow, BreakWindow .QWidget { background: "
                        "rgba(%1, %2, %3, %4); }")
                    .arg(backgroundColor.red())
                    .arg(backgroundColor.green())
                    .arg(backgroundColor.blue())
                    .arg(backgroundColor.alpha()));
}

void BreakWindow::start(int totalTime) {
  this->totalTime = totalTime;
  progressAnim->setDuration(totalTime * 1000);
  bgAnim->start();
  setTime(totalTime);
}

void BreakWindow::setTime(int remainingTime) {
  if (remainingTime == totalTime) {
    progressAnim->stop();
    progressAnim->start();
  }
  countdownLabel->setText(QString("%1").arg(round(float(remainingTime))));
}

void BreakWindow::setFullScreen() {
  setProperty("isFullScreen", true);
  setAttribute(Qt::WA_TransparentForMouseEvents, false);
  bgAnim->stop();
  setProperty("color", bgAnim->endValue());
  countdownLabel->setVisible(true);
  QPropertyAnimation *resizeAnim =
      new QPropertyAnimation(waylandWorkaround ? mainWidget : this, "geometry");
  resizeAnim->setStartValue(waylandWorkaround ? mainWidget->geometry() : geometry());
  resizeAnim->setEndValue(waylandWorkaround ? screen()->availableGeometry()
                                            : screen()->geometry());
  resizeAnim->setDuration(300);
  resizeAnim->start();
}

void BreakWindow::resizeToNormal() {
  setProperty("isFullScreen", false);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  bgAnim->start();
  countdownLabel->setVisible(false);
  QPropertyAnimation *resizeAnim =
      new QPropertyAnimation(waylandWorkaround ? mainWidget : this, "geometry");
  QRect rect = waylandWorkaround ? screen()->availableGeometry() : screen()->geometry();
  QRect targetGeometry = QRect(rect.x() + rect.width() / 2 - 150, rect.y(), 300, 100);
  resizeAnim->setStartValue(waylandWorkaround ? mainWidget->geometry() : geometry());
  resizeAnim->setEndValue(targetGeometry);
  resizeAnim->setDuration(100);
  resizeAnim->start();
}

void BreakWindow::initSize(QScreen *screen) {
  if (waylandWorkaround) {
    QRect rect = screen->availableGeometry();
    // Avoid using full height when initializing the main window. GNOME will refuse to
    // make the window always on top (done in custom shell extension) if it is too large
    // See https://askubuntu.com/questions/1122921.
    rect.setHeight(100);
    setGeometry(rect);
    mainWidget->setGeometry(rect.x() + rect.width() / 2 - 150, rect.y(), 300, 100);
  } else {
    QRect rect = screen->geometry();
    setGeometry(rect.x() + rect.width() / 2 - 150, rect.y(), 300, 100);
  }
  show();
  hide();
#ifdef LayerShellQt_FOUND
  if (QGuiApplication::platformName() == "wayland")
    if (auto window = LayerShellQt::Window::get(windowHandle())) {
      using namespace LayerShellQt;
      window->setCloseOnDismissed(true);
      window->setLayer(Window::LayerOverlay);
      window->setKeyboardInteractivity(Window::KeyboardInteractivityNone);
      window->setAnchors(Window::AnchorTop);
      // We do not want to reserve space for widgets like taskbar (#19)
      window->setExclusiveZone(-1);
    }
#elif defined Q_OS_MACOS
  macSetAllWorkspaces(windowHandle());
#endif
  // GNOME mutter will make the window black if show full screen
  // See https://gitlab.gnome.org/GNOME/mutter/-/issues/2520
  // GNOME mutter will also refuse to make a window always on top if maximized.
  // Therefore, we use the same `show()` with and without Wayland workaround.
  show();
}
