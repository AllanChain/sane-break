#ifndef SANE_BREAK_WINDOW_H
#define SANE_BREAK_WINDOW_H

#include <QMainWindow>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qtimer.h>

class BreakWindow : public QMainWindow {
  Q_OBJECT

public:
  BreakWindow(QWidget *parent = nullptr);
  ~BreakWindow();
  void tick();
  void onIdleStart();
  void onIdleEnd();
  bool shouldCountDown() { return isIdle || isForceBreak; }

private:
  bool isIdle = true;
  bool isForceBreak = false;
  int remainingTime;
  QTimer *countdownTimer;
  QProgressBar *progressBar;
  QLabel *countdownLabel;
};
#endif // SANE_BREAK_WINDOW_H
