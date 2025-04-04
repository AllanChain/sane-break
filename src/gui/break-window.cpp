// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "break-window.h"

#include <qglobal.h>

#include <QApplication>
#include <QColor>
#include <QFile>
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
#include "lib/flags.h"
#include "lib/preferences.h"
#include "lib/utils.h"

#ifdef Q_OS_LINUX
#include "lib/linux/system-check.h"
#endif  // Q_OS_LINUX
#ifdef WITH_LAYER_SHELL
#include <LayerShellQt/window.h>
#elif defined Q_OS_MACOS
#include "lib/macos/workspace.h"
#endif

const int BreakWindow::SMALL_WINDOW_WIDTH = 400;
const int BreakWindow::SMALL_WINDOW_HEIGHT = 120;

BreakWindow::BreakWindow(SanePreferences *preferences, QWidget *parent)
    : QMainWindow(parent), preferences(preferences) {
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
  setWindowTitle("Break reminder - Sane Break");
  setProperty("isFullScreen", false);

  mainWidget = new QWidget(this);
  if (!waylandWorkaround) setCentralWidget(mainWidget);
  mainWidget->setAttribute(Qt::WA_LayoutOnEntireRect);
  mainWidget->setContentsMargins(0, 0, 0, 0);

  // HACK: Not setting this at main.cpp because of
  // https://bugreports.qt.io/browse/QTBUG-133845
  QFile styleSheet(":/style.css");
  if (styleSheet.open(QIODevice::ReadOnly | QIODevice::Text))
    mainWidget->setStyleSheet(styleSheet.readAll());

  QVBoxLayout *layout = new QVBoxLayout(mainWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  QProgressBar *progressBar = new QProgressBar();
  progressBar->setMaximum(10000);
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);

  QVBoxLayout *textLayout = new QVBoxLayout();
  textLayout->setAlignment(Qt::AlignCenter);
  layout->addLayout(textLayout);

  breakLabel = new QLabel();
  breakLabel->setObjectName("breakLabel");
  textLayout->addWidget(breakLabel);

  countdownLabel = new QLabel(this);
  countdownLabel->setObjectName("countdownLabel");
  countdownLabel->setAlignment(Qt::AlignCenter);
  countdownLabel->setVisible(false);
  textLayout->addWidget(countdownLabel);

  if (preferences->textTransparency->get() > 0) {
    int opacity = 256 - preferences->textTransparency->get() * 256 / 100;
    countdownLabel->setStyleSheet(
        QString("color: rgba(236, 239, 244, %1)").arg(opacity));
    progressBar->setStyleSheet(
        QString("BreakWindow[isFullScreen=\"true\"] QProgressBar::chunk "
                "{background: rgba(236, 239, 244, %1)}")
            .arg(opacity));
  }

  progressAnim = new QPropertyAnimation(progressBar, "value");
  progressAnim->setStartValue(progressBar->maximum());
  progressAnim->setEndValue(0);

  bgAnim = new QPropertyAnimation(this, "color");
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

void BreakWindow::start(SaneBreak::BreakType type, int totalTime) {
  this->totalTime = totalTime;
  setTime(totalTime);
  progressAnim->setDuration(totalTime * 1000);

  if (type == SaneBreak::BreakType::Big) {
    breakLabel->setText(tr("Time for a big break"));
    bgAnim->setStartValue(QColor(180, 142, 173, 100));
  } else {
    breakLabel->setText(tr("Time for a small break"));
    bgAnim->setStartValue(QColor(235, 203, 139, 100));
  }
  bgAnim->setEndValue(QColor(46, 52, 64, 255));
  if (preferences->flashSpeed->get() <= 0) {
    bgAnim->setStartValue(QColor(46, 52, 64, 255));
    bgAnim->setDuration(100000);
  } else {
    bgAnim->setDuration(500 / preferences->flashSpeed->get() * 100);
  }
  bgAnim->start();
}

void BreakWindow::setTime(int remainingTime) {
  if (remainingTime == totalTime) {
    progressAnim->stop();
    progressAnim->start();
  }
  if (totalTime <= 60) {
    countdownLabel->setText(QString("%1").arg(remainingTime));
  } else {
    countdownLabel->setText(formatTime(remainingTime));
  }
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
  QRect targetGeometry = QRect(rect.x() + rect.width() / 2 - SMALL_WINDOW_WIDTH / 2,
                               rect.y(), SMALL_WINDOW_WIDTH, SMALL_WINDOW_HEIGHT);
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
    mainWidget->setGeometry(rect.x() + rect.width() / 2 - SMALL_WINDOW_WIDTH / 2,
                            rect.y(), SMALL_WINDOW_WIDTH, SMALL_WINDOW_HEIGHT);
  } else {
    QRect rect = screen->geometry();
    setGeometry(rect.x() + rect.width() / 2 - SMALL_WINDOW_WIDTH / 2, rect.y(),
                SMALL_WINDOW_WIDTH, SMALL_WINDOW_HEIGHT);
  }
  show();
  hide();
#ifdef WITH_LAYER_SHELL
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
