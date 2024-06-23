#ifndef SANE_TRAY_H
#define SANE_TRAY_H

#include <QObject>
#include <QSystemTrayIcon>

class TrayIcon : public QObject {
 public:
  TrayIcon();
  ~TrayIcon();

 private:
  QSystemTrayIcon *icon;
};

#endif  // SANE_TRAY_H
