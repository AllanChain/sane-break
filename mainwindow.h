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

private:
  bool isIdle = true;
  float remainingTime;
};
#endif // MAINWINDOW_H
