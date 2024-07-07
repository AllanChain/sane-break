#ifndef SANE_TRAY_H
#define SANE_TRAY_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include "break/winmanager.h"
#include "preferences/window.h"

class SaneBreakApp : public QObject {
  Q_OBJECT
 public:
  SaneBreakApp();
  ~SaneBreakApp();
  void start();
  int scheduleInterval();
  int breakTime();
  int smallBreaksBeforeBig();
  void breakNow();

 signals:
  void quit();

 private:
  PreferenceWindow *prefWindow;
  QSystemTrayIcon *icon;
  BreakWindowManager *breakManager;
  QTimer *countDownTimer;
  QMenu *menu;
  QAction *nextBreakAction;
  QAction *bigBreakAction;
  QAction *openPreferenceAction;
  QAction *quitAction;
  int breakCycleCount = 1;
  void createMenu();
  void tick();
  int secondsToNextBreak;
};

#endif  // SANE_TRAY_H
