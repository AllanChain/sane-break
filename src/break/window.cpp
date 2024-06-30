#include "window.h"

#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QRect>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>

BreakWindow::BreakWindow(QWidget *parent) : QMainWindow(parent) {
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setWindowFlags(Qt::ToolTip | Qt::WindowDoesNotAcceptFocus |
                 Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  setProperty("isFullScreen", false);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  centralWidget->setContentsMargins(10, 10, 10, 10);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);
  layout->setAlignment(Qt::AlignCenter);

  countdownLabel = new QLabel();
  countdownLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(countdownLabel);

  progressBar = new QProgressBar();
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);

  bgAnim = new QPropertyAnimation(this, "color");
  bgAnim->setStartValue(QColor(235, 203, 139, 100));
  bgAnim->setEndValue(QColor(46, 52, 64, 255));
  bgAnim->setDuration(500);
  bgAnim->setLoopCount(-1);
  bgAnim->start();
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

void BreakWindow::tick(int remainingTime) {
  if (!timeHasSet) {
    progressBar->setMaximum(remainingTime);
    timeHasSet = true;
  }
  progressBar->setValue(remainingTime);
  countdownLabel->setText(QString("Time remaining: %1 seconds")
                              .arg(round(float(remainingTime) / 1000)));
}

void BreakWindow::setFullScreen() {
  setProperty("isFullScreen", true);
  bgAnim->stop();
  setProperty("color", bgAnim->endValue());
  QPropertyAnimation *resizeAnim = new QPropertyAnimation(this, "geometry");
  resizeAnim->setStartValue(geometry());
  resizeAnim->setEndValue(screen()->geometry());
  resizeAnim->setDuration(300);
  resizeAnim->start();
}

void BreakWindow::resizeToNormal() {
  setProperty("isFullScreen", false);
  bgAnim->start();
  QPropertyAnimation *resizeAnim = new QPropertyAnimation(this, "geometry");
  QRect rect = screen()->geometry();
  QRect targetGeometry =
      QRect(rect.x() - rect.width() / 2 - 200, rect.y(), 300, 100);
  resizeAnim->setStartValue(geometry());
  resizeAnim->setEndValue(targetGeometry);
  resizeAnim->setDuration(100);
  resizeAnim->start();
}

void BreakWindow::initSize() {
  QRect rect = screen()->geometry();
  setGeometry(rect.x() - rect.width() / 2 - 200, rect.y(), 300, 100);
}
