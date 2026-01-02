// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <QContextMenuEvent>
#include <QLabel>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

#include "core/app.h"
#include "core/preferences.h"

class StatusTrayWindow : public QObject {
  Q_OBJECT

 public:
  StatusTrayWindow(SanePreferences* preferences, QObject* parent = nullptr);
  static StatusTrayWindow* createTrayOrWindow(SanePreferences* preferences,
                                              QObject* parent = nullptr);

  virtual void show() = 0;
  virtual void setTitle(QString str) = 0;
  virtual void update(TrayData data);
  QPixmap drawIcon(TrayData data);

  void onPostponeMinutesChange();

 signals:
  void nextBreakRequested();
  void nextBigBreakRequested();
  void smallBreakInsteadRequested();
  void enableBreakRequested();
  void preferenceWindowRequested();
  void postponeRequested(int secs);
  void quitRequested();

 protected:
  SanePreferences* preferences;
  QMenu* menu;
  QMenu* postponeMenu;
  QAction* quitAction;
  QAction* nextBreakAction;
  QAction* bigBreakAction;
  QAction* smallBreakInsteadAction;
  QAction* enableBreak;
};

class StatusTray : public StatusTrayWindow {
  Q_OBJECT

 public:
  StatusTray(SanePreferences* preferences, QObject* parent = nullptr);

  void show() override;
  void update(TrayData data) override;
  void setTitle(QString str) override;
  void onIconTrigger(QSystemTrayIcon::ActivationReason reason);

 private:
  QSystemTrayIcon* icon;
  QTimer* flashTimer;
  QPixmap emptyIconPixmap;
};

class StatusWindowWidget : public QWidget {
  Q_OBJECT

 public:
  StatusWindowWidget(QMenu* menu);
  QLabel* icon;
  QLabel* info;

 private:
  QMenu* menu;

 protected:
  void contextMenuEvent(QContextMenuEvent* event);
};

class StatusWindow : public StatusTrayWindow {
  Q_OBJECT

 public:
  StatusWindow(SanePreferences* preferences, QObject* parent = nullptr);
  ~StatusWindow();

  void show() override;
  void update(TrayData data) override;
  void setTitle(QString str) override;

 private:
  StatusWindowWidget* widget;

 protected:
  void contextMenuEvent(QContextMenuEvent* event);
};
