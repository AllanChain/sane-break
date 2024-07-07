#ifndef SANE_PREFERENCES_H
#define SANE_PREFERENCES_H

#include <QLabel>
#include <QMainWindow>
#include <QSlider>

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  PreferenceWindow(QWidget *parent = nullptr);
  void loadSettings();
  void saveSettings();

 private:
  void closeEvent(QCloseEvent *event);
  QSlider *smallBreakEverySlider;
  QLabel *smallBreakEveryLabel;
  QSlider *smallBreakForSlider;
  QLabel *smallBreakForLabel;
  QSlider *bigBreakAfterSlider;
  QLabel *bigBreakAfterLabel;
  QSlider *bigBreakForSlider;
  QLabel *bigBreakForLabel;
};
#endif  // SANE_PREFERENCES_H
