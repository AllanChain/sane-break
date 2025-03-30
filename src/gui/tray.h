// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_TRAY_H
#define SANE_TRAY_H
#include <QContextMenuEvent>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QWidget>

class StatusTrayWindow : public QObject {
  Q_OBJECT

 public:
  enum IconVariant {
    PAUSED = 1 << 0,
    WILL_BIG = 1 << 1,
  };
  Q_DECLARE_FLAGS(IconVariants, IconVariant);

 public:
  StatusTrayWindow(QMenu *menu, QObject *parent = nullptr)
      : menu(menu), QObject(parent) {};
  static StatusTrayWindow *createTrayOrWindow(QMenu *menu, QObject *parent = nullptr);
  QMenu *menu;
  virtual void show() {};
  virtual void updateIcon(float arcRatio, IconVariants flags) {};
  virtual void setTitle(QString str) {};
  QPixmap drawIcon(float arcRatio, IconVariants flags);
 signals:
  void breakTriggered();
};

class StatusTray : public StatusTrayWindow {
  Q_OBJECT

 public:
  StatusTray(QMenu *menu, QObject *parent = nullptr);
  void show();
  void updateIcon(float arcRatio, IconVariants flags);
  void setTitle(QString str);

 private:
  QSystemTrayIcon *icon;
};

class StatusWindowWidget : public QWidget {
  Q_OBJECT

 public:
  StatusWindowWidget(QMenu *menu);
  QLabel *icon;
  QLabel *info;

 private:
  QMenu *menu;

 protected:
  void contextMenuEvent(QContextMenuEvent *event);
};

class StatusWindow : public StatusTrayWindow {
  Q_OBJECT

 public:
  StatusWindow(QMenu *menu, QObject *parent = nullptr);
  ~StatusWindow();
  void show();
  void updateIcon(float arcRatio, IconVariants flags);
  void setTitle(QString str);

 private:
  StatusWindowWidget *widget;

 protected:
  void contextMenuEvent(QContextMenuEvent *event);
};

#endif  // SANE_TRAY_H
