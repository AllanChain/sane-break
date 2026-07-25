// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <QColor>
#include <QContextMenuEvent>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QObject>
#include <QPixmap>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>
#include <optional>

#include "core/app.h"
#include "core/preferences.h"

struct TrayArcSpec {
  QColor dark;
  QColor light;
  float ratio;
};

struct TrayIconSpec {
  QString baseIcon;
  std::optional<TrayArcSpec> arc;
  std::optional<QColor> dot;
};

TrayIconSpec trayIconSpec(TrayData data);
QPixmap renderTrayIcon(TrayIconSpec spec);

class StatusTrayWindow : public QObject {
  Q_OBJECT

 public:
  StatusTrayWindow(SanePreferences* preferences, QObject* parent = nullptr);
  static StatusTrayWindow* createTrayOrWindow(SanePreferences* preferences,
                                              QObject* parent = nullptr);

  virtual void show() = 0;
  virtual void setTitle(QString str) = 0;
  virtual void update(TrayData data);

 signals:
  void nextBreakRequested();
  void nextBigBreakRequested();
  void smallBreakInsteadRequested();
  void enableBreakRequested();
  void preferenceWindowRequested();
  void postponeRequested();
  void postponePresetRequested(int minutes);
  void meetingRequested();
  void meetingPresetRequested(int minutes);
  void endMeetingRequested();
  void extendMeetingRequested(int seconds);
  void focusRequested();
  void focusPresetRequested(int minutes);
  void endFocusRequested();
  void statsRequested();
  void quitRequested();

 public:
  static QList<int> postponePresets(int maxMinutes);
  static QList<int> focusPresets(int focusSmallEveryMinutes);

  QMenu* getPostponeMenu() const { return postponeMenu; }
  QMenu* getMeetingMenu() const { return meetingMenu; }
  QMenu* getFocusMenu() const { return focusMenu; }

 protected:
  SanePreferences* preferences;
  QMenu* menu;
  QMenu* postponeMenu;
  QMenu* meetingMenu;
  QAction* endMeetingAction;
  QMenu* extendMeetingMenu;
  struct ExtendOption {
    QAction* action;
    int seconds;
  };
  QList<ExtendOption> extendOptions;
  QMenu* focusMenu;
  QAction* endFocusAction;
  QAction* quitAction;
  QAction* nextBreakAction;
  QAction* bigBreakAction;
  QAction* smallBreakInsteadAction;
  QAction* enableBreak;

  void buildPostponeMenu();
  void buildMeetingMenu();
  void buildFocusMenu();
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
