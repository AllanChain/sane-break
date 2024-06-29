#ifndef SANE_TRAY_H
#define SANE_TRAY_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include "break/winmanager.h"

class SaneBreakApp : public QObject {
  Q_OBJECT
 public:
  SaneBreakApp();
  ~SaneBreakApp();
  void start();
  int scheduleInterval();
  int breakTime();
  void breakNow();

 signals:
  void quit();

 private:
  QSystemTrayIcon *icon;
  BreakWindowManager *breakManager;
  QTimer *countDownTimer;
  QMenu *menu;
  QAction *quitAction;
  QAction *nextBreakAction;
  QAction *breakNowAction;
  void createMenu();
  void tick();
  int secondsToNextBreak;
};

#endif  // SANE_TRAY_H
