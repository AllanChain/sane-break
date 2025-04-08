// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "break-window.h"

#include <qglobal.h>

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QIODevice>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
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
#include "core/window-control.h"
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

BreakWindow::BreakWindow(BreakData data, QWidget *parent)
    : AbstractBreakWindow(data, parent) {
#ifdef Q_OS_LINUX
  // Positioning windows on Wayland is nearly impossible without layer shell protol.
  // In Wayland workaround mode, the main window is transparent and takes up all
  // available space on the screen (but not the system panels), and the main widget
  // changes size and covers the space.
  waylandWorkaround =
      QGuiApplication::platformName() == "wayland" && !LinuxSystemSupport::layerShell;
#endif
  setAttribute(Qt::WA_TranslucentBackground);  // transparent window
  setAttribute(Qt::WA_ShowWithoutActivating);  // avoid gaining keyboard focus
  setAttribute(Qt::WA_LayoutOnEntireRect);     // ignore safe zone on macOS
  setWindowFlags(Qt::ToolTip | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput);
  setWindowTitle("Break reminder - Sane Break");
  setProperty("isFullScreen", false);

  mainWidget = new QWidget(this);
  if (!waylandWorkaround) setCentralWidget(mainWidget);
  mainWidget->setAttribute(Qt::WA_LayoutOnEntireRect);
  mainWidget->setContentsMargins(0, 0, 0, 0);
  mainWidget->setStyleSheet(QString(R"(
BreakWindow QLabel#breakLabel {
  color: %1;
  background: transparent;
  font-size: 20px;
}
BreakWindow[isFullScreen="true"] QLabel#breakLabel {
  font-size: 40px;
}
BreakWindow QLabel#countdownLabel {
  color: %2;
  background: transparent;
}
BreakWindow[isFullScreen="true"] QLabel#countdownLabel {
  font-size: 100px;
}
BreakWindow QLabel#killTip {
  color: %1;
  background: transparent;
  margin-top: 30px;
  font-size: 20px;
}

BreakWindow QProgressBar {
  border-radius: 5px;
  max-height: 5px;
  background: transparent;
}
BreakWindow QProgressBar::chunk {
  background: %2;
  width: 1px;
}
BreakWindow[isFullScreen="true"] QProgressBar {
  max-height: 10px;
})")
                                .arg(data.theme.messageColor.name(QColor::HexArgb),
                                     data.theme.countDownColor.name(QColor::HexArgb)));

  QVBoxLayout *layout = new QVBoxLayout(mainWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  QProgressBar *progressBar = new QProgressBar();
  progressBar->setMaximum(10000);
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);

  QVBoxLayout *textLayout = new QVBoxLayout();
  textLayout->setAlignment(Qt::AlignCenter);
  layout->addLayout(textLayout);

  breakLabel = new QLabel(data.message);
  breakLabel->setObjectName("breakLabel");
  breakLabel->setAlignment(Qt::AlignCenter);
  textLayout->addWidget(breakLabel);

  countdownLabel = new QLabel();
  countdownLabel->setObjectName("countdownLabel");
  countdownLabel->setAlignment(Qt::AlignCenter);
  countdownLabel->setVisible(false);
  textLayout->addWidget(countdownLabel);

  killTip = new QLabel();
  killTip->setObjectName("killTip");
  killTip->setVisible(false);
  killTip->setMinimumWidth(600);
  killTip->setAlignment(Qt::AlignCenter);
  killTip->setWordWrap(true);
  QString tipText = tr("<p>Sane Break is in force break mode.</p>");
#ifdef Q_OS_LINUX
  tipText +=
      tr("<p>Quit Sane Break by running <code>killall sane-break</code> in "
         "terminal.</p>");
#elif defined Q_OS_MACOS
  tipText +=
      tr("<p>Quit Sane Break by enabling Spotlight with <code>Cmd + Space</code>, "
         "opening terminal, and running <code>killall sane-break</code>.</p>");
#elif defined Q_OS_WINDOWS
  tipText +=
      tr("<p>Quit Sane Break by pressing <code>Win + R</code> and run "
         "<code>taskkill /IM sane-break.exe /F</code>");
#endif
  killTip->setText(tipText);
  textLayout->addWidget(killTip);

  progressAnim = new QPropertyAnimation(progressBar, "value");
  progressAnim->setStartValue(progressBar->maximum());
  progressAnim->setEndValue(0);

  this->totalTime = data.totalSeconds;
  progressAnim->setDuration(totalTime * 1000);

  bgAnim = new QPropertyAnimation(this, "color");
  bgAnim->setStartValue(data.theme.highlightBackground);
  bgAnim->setEndValue(data.theme.mainBackground);
  bgAnim->setDuration(data.theme.flashAnimationDuration);
  bgAnim->setLoopCount(-1);
}

void BreakWindow::colorChanged() {
  setStyleSheet(QString("BreakWindow, BreakWindow .QWidget { background: "
                        "rgba(%1, %2, %3, %4); }")
                    .arg(backgroundColor.red())
                    .arg(backgroundColor.green())
                    .arg(backgroundColor.blue())
                    .arg(backgroundColor.alpha()));
}

void BreakWindow::start() {
  setTime(totalTime);
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

void BreakWindow::showKillTip() { killTip->setVisible(true); }

void BreakWindow::setFullScreen() {
  setProperty("isFullScreen", true);
  setWindowFlag(Qt::WindowTransparentForInput, false);
  show();  // required for modifying window flags
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
  setWindowFlag(Qt::WindowTransparentForInput);
  show();  // required for modifying window flags
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
  createWinId();
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
