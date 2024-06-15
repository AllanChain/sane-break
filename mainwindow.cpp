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

int totalTime = 10 * 1000;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  setStyleSheet("background: rgba(0, 0, 0, 30%);");
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setWindowFlags(Qt::Dialog | Qt::WindowDoesNotAcceptFocus |
                 Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

  resize(300, 100);
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
  layout->addWidget(progressBar);

  remainingTime = totalTime;

  progressBar->setValue(remainingTime);
  countdownLabel->setText(
      QString("Time remaining: %1 seconds").arg(remainingTime));

  QTimer *timer = new QTimer(this); // Create timer
  timer->setInterval(10);

  QObject::connect(timer, &QTimer::timeout, [=]() {
    if (this->isIdle) {
      this->remainingTime -= 10;
    }
    progressBar->setValue(this->remainingTime);
    countdownLabel->setText(QString("Time remaining: %1 seconds")
                                .arg(round(this->remainingTime / 1000)));
    if (this->remainingTime <= 0) {
      timer->stop();
      this->close();
    }
  });

  timer->start(); // Start timer
}

MainWindow::~MainWindow() {}

void MainWindow::onIdleStart() { this->isIdle = true; }
void MainWindow::onIdleEnd() {
  this->isIdle = false;
  this->remainingTime = totalTime;
}
