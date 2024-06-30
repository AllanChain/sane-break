#ifndef SANE_BREAK_WINDOW_H
#define SANE_BREAK_WINDOW_H

#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>

class BreakWindow : public QMainWindow {
  Q_OBJECT
  Q_PROPERTY(QColor color MEMBER backgroundColor NOTIFY colorChanged)

 public:
  BreakWindow(QWidget *parent = nullptr);
  ~BreakWindow();
  void tick(int remainingTime);
  void setFullScreen();
  void resizeToNormal();
  void initSize();
  void colorChanged();

 private:
  QProgressBar *progressBar;
  QLabel *countdownLabel;
  QColor backgroundColor;
  QPropertyAnimation *bgAnim;
  bool timeHasSet = false;
};
#endif  // SANE_BREAK_WINDOW_H
