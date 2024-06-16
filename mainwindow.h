#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qtimer.h>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void tick();
  void onIdleStart();
  void onIdleEnd();
  bool shouldCountDown() { return isIdle || isForceBreak; }

private:
  bool isIdle = true;
  bool isForceBreak = false;
  int remainingTime;
  QTimer *countdownTimer;
  QTimer *forceBreakTimer;
  QProgressBar *progressBar;
  QLabel *countdownLabel;
};
#endif // MAINWINDOW_H
