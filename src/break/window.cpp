#include "window.h"

#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>

BreakWindow::BreakWindow(QWidget *parent) : QMainWindow(parent) {
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
  progressBar->setTextVisible(false);
  layout->addWidget(progressBar);
}

BreakWindow::~BreakWindow() {}

void BreakWindow::tick(int remainingTime) {
  if (!timeHasSet) {
    progressBar->setMaximum(remainingTime);
    timeHasSet = true;
  }
  progressBar->setValue(remainingTime);
  countdownLabel->setText(QString("Time remaining: %1 seconds")
                              .arg(round(float(remainingTime) / 1000)));
  if (isSmallWindow)
    setStyleSheet(QString("background: rgba(235, 203, 139, %1);")
                      .arg(sin(remainingTime * 3.14 / 1000) / 5 + 0.5));
}

void BreakWindow::setFullScreen() {
  setStyleSheet("background: #2e3440;");
  setGeometry(screen()->geometry());
  isSmallWindow = false;
}

void BreakWindow::resizeToNormal() {
  resize(300, 100);
  isSmallWindow = true;
}
