#ifndef SANE_BREAK_WINDOW_H
#define SANE_BREAK_WINDOW_H

#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>

class BreakWindow : public QMainWindow {
  Q_OBJECT

 public:
  BreakWindow(QWidget *parent = nullptr);
  ~BreakWindow();
  void setTime(int remainingTime);
  void tickWarning(int remainingTime);
  void setFullScreen();
  void resizeToNormal();

 private:
  QProgressBar *progressBar;
  QLabel *countdownLabel;
  bool timeHasSet = false;
};
#endif  // SANE_BREAK_WINDOW_H
