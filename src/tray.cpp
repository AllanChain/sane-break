#include "tray.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

TrayIcon::TrayIcon() : QObject() {
  icon = new QSystemTrayIcon(this);
  QMenu *menu = new QMenu();
  menu->addAction(new QAction("Help", this));
  icon->setIcon(QIcon(":/images/icon.png"));
  icon->setContextMenu(menu);
  icon->show();
}
TrayIcon::~TrayIcon() {}
