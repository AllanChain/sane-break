#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void onIdleStart();
  void onIdleEnd();
  bool shouldCountDown() { return isIdle || isForceBreak; }

private:
  bool isIdle = true;
  bool isForceBreak = false;
  int remainingTime;
};
#endif // MAINWINDOW_H
