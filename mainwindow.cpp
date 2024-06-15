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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  setStyleSheet("background: rgba(0, 0, 0, 30%);");
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating);
  setWindowFlags(Qt::Dialog | Qt::WindowDoesNotAcceptFocus |
                 Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);

  QLabel *label = new QLabel("Hello World");
  label->setAlignment(Qt::AlignCenter);
  layout->addWidget(label);

  int totalTime = 4 * 1000;

  QProgressBar *progressBar = new QProgressBar(); // Create progress bar
  progressBar->setMaximum(totalTime);
  layout->addWidget(progressBar);

  QLabel *countdownLabel = new QLabel(); // Create countdown label
  countdownLabel->setAlignment(Qt::AlignCenter);
  layout->addWidget(countdownLabel);

  float remainingTime = totalTime;

  progressBar->setValue(remainingTime);
  countdownLabel->setText(
      QString("Time remaining: %1 seconds").arg(remainingTime));

  QTimer *timer = new QTimer(this); // Create timer
  timer->setInterval(10);

  QObject::connect(timer, &QTimer::timeout, [=]() mutable {
    remainingTime -= 10;
    progressBar->setValue(remainingTime);
    countdownLabel->setText(
        QString("Time remaining: %1 seconds").arg(round(remainingTime / 1000)));
    if (remainingTime <= 0) {
      timer->stop();
      this->close();
    }
  });

  timer->start(); // Start timer
}

MainWindow::~MainWindow() {}
