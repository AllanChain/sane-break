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
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>
#include <Qt>
#include <cmath>

#include "config.h"
#include "core/window-control.h"
#include "lib/utils.h"
#include "ui_break-window.h"

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
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setWindowFlags(Qt::ToolTip | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput);
  setWindowTitle("Break reminder - Sane Break");

  mainWidget = new QWidget(this);
  if (!waylandWorkaround) setCentralWidget(mainWidget);

  ui = new Ui::BreakReminder();
  ui->setupUi(mainWidget);
  mainWidget->resize(0, 0);
  mainWidget->setStyleSheet(
      QString("BreakWindow QLabel { color: %1; }"
              "BreakWindow #countdownLabel { color: %2; }"
              "BreakWindow QProgressBar::chunk { background: %2; }")
          .arg(data.theme.messageColor.name(QColor::HexArgb),
               data.theme.countDownColor.name(QColor::HexArgb)));
  mainWidget->setProperty("isFullScreen", false);
  mainWidget->setAttribute(Qt::WA_LayoutOnEntireRect);

  ui->breakLabel->setText(data.message);
  ui->countdownLabel->setVisible(false);
  ui->buttons->setVisible(false);
  colorizeButton(ui->lockScreen, data.theme.messageColor);
  colorizeButton(ui->exitForceBreak, data.theme.messageColor);
  connect(ui->lockScreen, &QToolButton::pressed, this,
          &AbstractBreakWindow::lockScreenRequested);
  connect(ui->exitForceBreak, &QToolButton::pressed, this,
          &AbstractBreakWindow::exitForceBreakRequested);

  progressAnim = new QPropertyAnimation(ui->progressBar, "value");
  progressAnim->setStartValue(ui->progressBar->maximum());
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
  setStyleSheet(QString("BreakWindow, BreakWindow #BreakReminder { background: "
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
    ui->countdownLabel->setText(QString("%1").arg(remainingTime));
  } else {
    ui->countdownLabel->setText(formatTime(remainingTime));
  }
}

void BreakWindow::showButtons(bool show) { ui->buttons->setVisible(show); }

void BreakWindow::setFullScreen() {
  mainWidget->setProperty("isFullScreen", true);
  setAttribute(Qt::WA_TransparentForMouseEvents, false);
  // Removing WA_TransparentForMouseEvents does not automatically removes
  // the window flag WindowTransparentForInput
  setWindowFlag(Qt::WindowTransparentForInput, false);
  show();  // required for modifying window flags
  bgAnim->stop();
  setProperty("color", bgAnim->endValue());
  ui->countdownLabel->setVisible(true);
  QPropertyAnimation *resizeAnim =
      new QPropertyAnimation(waylandWorkaround ? mainWidget : this, "geometry");
  resizeAnim->setStartValue(waylandWorkaround ? mainWidget->geometry() : geometry());
  resizeAnim->setEndValue(waylandWorkaround ? screen()->availableGeometry()
                                            : screen()->geometry());
  resizeAnim->setDuration(300);
  resizeAnim->start();
}

void BreakWindow::resizeToNormal() {
  mainWidget->setProperty("isFullScreen", false);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  bgAnim->start();
  ui->countdownLabel->setVisible(false);
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

void BreakWindow::colorizeButton(QToolButton *button, QColor color) {
  auto pixmap = button->icon().pixmap(button->iconSize());
  auto mask = pixmap.mask();
  pixmap.fill(color);
  pixmap.setMask(mask);
  button->setIcon(pixmap);
}
