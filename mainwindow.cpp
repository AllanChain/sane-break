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
#include <qwindowdefs.h>

int totalTime = 10 * 1000;

#define FRAME_RATE 25

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  setStyleSheet("background: rgba(59, 66, 82, 50%);");
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setWindowFlags(Qt::Dialog | Qt::WindowDoesNotAcceptFocus |
                 Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);

  QLabel *countdownLabel = new QLabel(); // Create countdown label
  countdownLabel->setStyleSheet("background: transparent;");
  countdownLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(countdownLabel);

  QProgressBar *progressBar = new QProgressBar(); // Create progress bar
  progressBar->setStyleSheet("background: transparent;");
  progressBar->setMaximum(totalTime);
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);

  progressBar->setValue(totalTime);
  countdownLabel->setText(QString("Time remaining: %1 seconds").arg(totalTime));

  remainingTime = totalTime;

  QTimer *countdownTimer = new QTimer(this);
  countdownTimer->setInterval(1000 / FRAME_RATE);

  QTimer *forceBreakTimer = new QTimer(this);
  forceBreakTimer->setInterval(30 * 1000);
  forceBreakTimer->setSingleShot(true);

  QObject::connect(countdownTimer, &QTimer::timeout, [=]() {
    this->remainingTime -= 1000 / FRAME_RATE;
    if (!this->shouldCountDown()) { // Keep resetting time if not idle
      if (this->remainingTime < totalTime - 500) {
        this->remainingTime = totalTime;
      }
      setStyleSheet(QString("background: rgba(235, 203, 139, %1);")
                        .arg(sin(this->remainingTime * 3.14 / 1000) / 5 + 0.5));
    }
    progressBar->setValue(this->remainingTime);
    countdownLabel->setText(QString("Time remaining: %1 seconds")
                                .arg(round(float(this->remainingTime) / 1000)));
    if (this->remainingTime <= 0) {
      countdownTimer->stop();
      this->close();
    }
  });
  QObject::connect(forceBreakTimer, &QTimer::timeout, [=]() {
    setStyleSheet("background: black;");
    this->setGeometry(this->screen()->geometry());
    this->isForceBreak = true;
  });

  countdownTimer->start();
  forceBreakTimer->start();
}

MainWindow::~MainWindow() {}

void MainWindow::onIdleStart() {
  if (this->isForceBreak)
    return;
  setStyleSheet("background: rgba(0, 0, 0, 30%);");
  this->isIdle = true;
}
void MainWindow::onIdleEnd() {
  if (this->isForceBreak)
    return;
  this->isIdle = false;
  this->remainingTime = totalTime;
}
