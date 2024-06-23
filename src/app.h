#ifndef SANE_TRAY_H
#define SANE_TRAY_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include "break/scheduler.h"

class SaneBreakApp : public QObject {
  Q_OBJECT
 public:
  SaneBreakApp();
  ~SaneBreakApp();
  void start();

 signals:
  void quit();

 private slots:
  void updateNextBreakTime();

 private:
  QSystemTrayIcon *icon;
  BreakScheduler *breakScheduler;
  QTimer *updateTimer;
  QMenu *menu;
  QAction *quitAction;
  QAction *nextBreakAction;
  void createMenu();
};

#endif  // SANE_TRAY_H
