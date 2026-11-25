// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QList>
#include <QMainWindow>
#include <QPointer>
#include <QString>

#include "core/flags.h"

class AutoStart;
class BreakWindows;
class ControllerHolder;
class QCloseEvent;
class QComboBox;
class QEvent;
class QLineEdit;
class QPushButton;
class QShowEvent;
class QTimer;
class QWidget;
class SanePreferences;
class SoundPlayer;
class TextWindow;

namespace Ui {
class PrefWindow;
}

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit PreferenceWindow(SanePreferences* preferences, QWidget* parent = nullptr);
  ~PreferenceWindow() override;

 private:
  enum class PreviewStep {
    Idle,
    SmallFullScreen,
    BigPrompt,
    BigFullScreen,
    Finish,
  };

  Ui::PrefWindow* ui;
  SanePreferences* preferences;
  QList<QPushButton*> tabButtons;
  SoundPlayer* soundPlayer;
  ControllerHolder* controllers;
  AutoStart* autoStart;
  QPointer<TextWindow> openingTextWindow;
  BreakWindows* breakWindows;
  QTimer* previewTimer;
  PreviewStep previewStep = PreviewStep::Idle;

  void setupTabs();
  void setupScheduleTab();
  void setupReminderTab();
  void setupInterfaceTab();
  void setupPauseTab();
  void setupSoundTab();
  void setupGeneralTab();
  void setupAboutTab();
  void setupControlBar();
  void syncBigBreakEnabled();

  bool confirmLeave();
  void setTab(int tabNum, int totalTabCount);
  void forceThemeUpdate();
  void openNotice();
  void openSourceCode();
  void openWeblate();
  void browseForSound(QComboBox* comboBox);
  void browseForBackgroundImage(QLineEdit* target, BreakType type);
  // Copies `file` into `<app data>/<subdirectory>` and returns the destination path,
  // or an empty string after warning the user.
  QString importFileToAppData(const QString& file, const QString& subdirectory);
  void openBreakWindowPreview();
  void advanceBreakWindowPreview();
  void stopBreakWindowPreview();
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;
  void changeEvent(QEvent* event) override;
};
