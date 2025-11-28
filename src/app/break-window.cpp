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

#include "config.h"
#include "core/break-windows.h"
#include "lib/utils.h"
#include "ui_break-window.h"

#ifdef Q_OS_LINUX
#include "lib/linux/system-check.h"
#endif  // Q_OS_LINUX
#ifdef Q_OS_MACOS
#include "lib/macos/workspace.h"
#endif

const int BreakWindow::SMALL_WINDOW_WIDTH = 400;
const int BreakWindow::SMALL_WINDOW_HEIGHT = 120;

BreakWindow::BreakWindow(BreakWindowData data, QWidget* parent)
    : QMainWindow(parent), m_data(data) {
#ifdef Q_OS_LINUX
  // Positioning windows on Wayland is nearly impossible without layer shell protol.
  // In Wayland workaround mode, the main window is transparent and takes up all
  // available space on the screen (but not the system panels), and the main widget
  // changes size and covers the space.
  m_waylandWorkaround =
      QGuiApplication::platformName() == "wayland" && !LinuxSystemSupport::layerShell;
  // X11 does not support changing WindowTransparentForInput flag afterwards.
  // Therefore, we just don't set the WindowTransparentForInput flag.
  m_supportTransparentInput = QGuiApplication::platformName() == "wayland";
#endif
  setAttribute(Qt::WA_TranslucentBackground);    // transparent window
  setAttribute(Qt::WA_ShowWithoutActivating);    // avoid gaining keyboard focus
  setAttribute(Qt::WA_LayoutOnEntireRect);       // ignore safe zone on macOS
  setAttribute(Qt::WA_MacAlwaysShowToolWindow);  // always show window on macOS
  setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint);
#ifdef Q_OS_MACOS  // Use Qt::ToolTip on macOS to display above menubars
  setWindowFlag(Qt::ToolTip);
#else  // Use Qt::Tool on other platforms to avoid glitches in #79
  setWindowFlag(Qt::Tool);
#endif
  if (m_supportTransparentInput) setWindowFlag(Qt::WindowTransparentForInput);
  setWindowTitle("Break reminder - Sane Break");

  mainWidget = new QWidget(this);
  if (!m_waylandWorkaround) setCentralWidget(mainWidget);

  ui = new Ui::BreakReminder();
  ui->setupUi(mainWidget);
  mainWidget->resize(0, 0);
  mainWidget->setStyleSheet(
      QString("BreakWindow QLabel { color: %1; }"
              "BreakWindow #countdownLabel { color: %2; }"
              "BreakWindow #breakEndTimeLabel { color: %2; }"
              "BreakWindow QProgressBar::chunk { background: %2; }")
          .arg(data.theme.messageColor.name(QColor::HexArgb),
               data.theme.countDownColor.name(QColor::HexArgb)));
  mainWidget->setProperty("isFullScreen", false);
  mainWidget->setAttribute(Qt::WA_LayoutOnEntireRect);

  QColor hoverColor = data.theme.messageColor;
  hoverColor.setAlpha(40);
  ui->buttons->setStyleSheet(
      ui->buttons->styleSheet().replace("#aaaaaaaa", hoverColor.name(QColor::HexArgb)));
  colorizeButton(ui->lockScreen, data.theme.messageColor);
  colorizeButton(ui->exitForceBreak, data.theme.messageColor);
  ui->lockScreenGroup->setVisible(false);
  ui->exitForceBreakGroup->setVisible(false);
  connect(ui->lockScreen, &QPushButton::pressed, this,
          &BreakWindow::lockScreenRequested);
  connect(ui->exitForceBreak, &QPushButton::pressed, this,
          &BreakWindow::exitForceBreakRequested);

  m_progressAnim = new QPropertyAnimation(ui->progressBar, "value");
  m_progressAnim->setStartValue(ui->progressBar->maximum());
  m_progressAnim->setEndValue(0);

  this->m_totalSeconds = data.totalSeconds;
  m_progressAnim->setDuration(m_totalSeconds * 1000);
  m_progressAnim->start();

  m_bgAnim = new QPropertyAnimation(this, "color");
  m_bgAnim->setStartValue(data.theme.highlightBackground);
  m_bgAnim->setEndValue(data.theme.mainBackground);
  m_bgAnim->setDuration(data.theme.flashAnimationDuration);
  m_bgAnim->setLoopCount(-1);
  m_bgAnim->start();
}

void BreakWindow::colorChanged() {
  setStyleSheet(QString("BreakWindow, BreakWindow #BreakReminder { background: "
                        "rgba(%1, %2, %3, %4); }")
                    .arg(backgroundColor.red())
                    .arg(backgroundColor.green())
                    .arg(backgroundColor.blue())
                    .arg(backgroundColor.alpha()));
}

void BreakWindow::setTime(int remainingTime, QString estimatedEndTime) {
  m_progressAnim->setCurrentTime((m_totalSeconds - remainingTime) * 1000);
  if (m_totalSeconds <= 60) {
    ui->countDownLabel->setText(QString("%1").arg(remainingTime));
  } else {
    ui->countDownLabel->setText(formatTime(remainingTime));
  }
  if (remainingTime > 0) {
    ui->breakEndTimeLabel->setText(tr("Break will end at: %1").arg(estimatedEndTime));
  } else {
    ui->breakEndTimeLabel->setText(tr("Break has ended"));
  }
}

void BreakWindow::setClock(QString hourMinute) { ui->clock->setText(hourMinute); }

void BreakWindow::showButtons(AbstractBreakWindows::Buttons buttons, bool show) {
  // Lock screen is not supported on Flatpak
#ifndef LINUX_DIST_FLATPAK
  if (buttons.testFlag(AbstractBreakWindows::Button::LockScreen))
    ui->lockScreenGroup->setVisible(show);
#endif
  if (buttons.testFlag(AbstractBreakWindows::Button::ExitForceBreak))
    ui->exitForceBreakGroup->setVisible(show);
}

void BreakWindow::showFullScreen() {
  mainWidget->setProperty("isFullScreen", true);
  if (m_supportTransparentInput) {
    // setWindowFlag will reparent the window and do a lot of unnecessary works
    // Instead, we just change the flags of QWindow
    windowHandle()->setFlag(Qt::WindowTransparentForInput, false);
  }
  m_bgAnim->stop();
  setProperty("color", m_bgAnim->endValue());

  ui->breakLabel->setText(m_data.message.fullScreen);
  if (m_data.show.countdown) ui->countDownLabel->setVisible(true);
  if (m_data.show.clock) {
    ui->clock->setVisible(true);
    ui->clock->setStyleSheet(
        QString("QLabel { background: transparent; font-size: %1px; }")
            .arg(m_data.show.countdown ? 50 : 100));
  }
  if (m_data.show.endTime) ui->breakEndTimeLabel->setVisible(true);
  if (m_data.show.buttons) ui->buttons->setVisible(true);

  QPropertyAnimation* resizeAnim =
      new QPropertyAnimation(m_waylandWorkaround ? mainWidget : this, "geometry");
  resizeAnim->setStartValue(m_waylandWorkaround ? mainWidget->geometry() : geometry());
  resizeAnim->setEndValue(m_waylandWorkaround ? screen()->availableGeometry()
                                              : screen()->geometry());
  resizeAnim->setDuration(300);
  resizeAnim->start();
}

void BreakWindow::showFlashPrompt() {
  mainWidget->setProperty("isFullScreen", false);
  if (m_supportTransparentInput) {
    windowHandle()->setFlag(Qt::WindowTransparentForInput, true);
  }
  m_bgAnim->start();

  ui->breakLabel->setText(m_data.message.prompt);
  if (!m_data.show.prograssBar) {
    ui->progressBar->setVisible(false);
  }
  ui->countDownLabel->setVisible(false);
  ui->clock->setVisible(false);
  ui->breakEndTimeLabel->setVisible(false);
  ui->buttons->setVisible(false);

  QPropertyAnimation* resizeAnim =
      new QPropertyAnimation(m_waylandWorkaround ? mainWidget : this, "geometry");
  QRect rect =
      m_waylandWorkaround ? screen()->availableGeometry() : screen()->geometry();
  QRect targetGeometry = QRect(rect.x() + rect.width() / 2 - SMALL_WINDOW_WIDTH / 2,
                               rect.y(), SMALL_WINDOW_WIDTH, SMALL_WINDOW_HEIGHT);
  resizeAnim->setStartValue(m_waylandWorkaround ? mainWidget->geometry() : geometry());
  resizeAnim->setEndValue(targetGeometry);
  resizeAnim->setDuration(100);
  resizeAnim->start();
}

void BreakWindow::initSize(QScreen* screen) {
  if (m_waylandWorkaround) {
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
#ifdef Q_OS_MACOS
  macSetAllWorkspaces(windowHandle());
#endif
}

void BreakWindow::colorizeButton(QPushButton* button, QColor color) {
  auto pixmap = button->icon().pixmap(button->iconSize());
  auto mask = pixmap.mask();
  pixmap.fill(color);
  pixmap.setMask(mask);
  button->setIcon(pixmap);
}
