#include "mainwindow.h"

#include <LayerShellQt/Shell>
#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>
#include <cmath>
#include <qtimer.h>
#include <qwindowdefs.h>

int totalTime = 10 * 1000;

#define FRAME_RATE 25

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  setStyleSheet("background: rgb(59, 66, 82);");
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setWindowFlags(Qt::ToolTip | Qt::WindowDoesNotAcceptFocus |
                 Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);

  countdownLabel = new QLabel();
  countdownLabel->setStyleSheet("background: transparent;");
  countdownLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(countdownLabel);

  progressBar = new QProgressBar();
  progressBar->setStyleSheet("background: transparent;");
  progressBar->setMaximum(totalTime);
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);

  progressBar->setValue(totalTime);
  countdownLabel->setText(QString("Time remaining: %1 seconds").arg(totalTime));
  remainingTime = totalTime;

  countdownTimer = new QTimer(this);
  countdownTimer->setInterval(1000 / FRAME_RATE);

  QTimer::singleShot(30 * 1000, [=]() { // Force break
    setStyleSheet("background: #2e3440;");
    this->setGeometry(this->screen()->geometry());
    this->isForceBreak = true;
  });
  QTimer::singleShot(2000, [=]() { // Go fullscreen when idle for 2 sec
    if (isIdle) {
      setStyleSheet("background: #2e3440;");
      this->setGeometry(this->screen()->geometry());
    }
  });

  QObject::connect(countdownTimer, &QTimer::timeout, this, &MainWindow::tick);

  countdownTimer->start();
}

MainWindow::~MainWindow() {}

void MainWindow::tick() {
  remainingTime -= 1000 / FRAME_RATE;
  if (!shouldCountDown()) { // Keep resetting time if not idle
    if (remainingTime < totalTime - 500) {
      remainingTime = totalTime;
    }
    setStyleSheet(QString("background: rgba(235, 203, 139, %1);")
                      .arg(sin(remainingTime * 3.14 / 1000) / 5 + 0.5));
  }
  progressBar->setValue(this->remainingTime);
  countdownLabel->setText(QString("Time remaining: %1 seconds")
                              .arg(round(float(remainingTime) / 1000)));
  if (remainingTime <= 0) {
    countdownTimer->stop();
    destroy();
  }
}

void MainWindow::onIdleStart() {
  if (this->isForceBreak)
    return;
  setGeometry(this->screen()->geometry());
  setStyleSheet("background: rgb(59, 66, 82);");
  this->isIdle = true;
}
void MainWindow::onIdleEnd() {
  if (this->isForceBreak)
    return;
  resize(300, 100);
  this->isIdle = false;
  this->remainingTime = totalTime;
}
