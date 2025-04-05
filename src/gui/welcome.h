// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QLabel>
#include <QWidget>

#include "config.h"
#include "lib/preferences.h"
#include "widgets/language-select.h"

class WelcomeWindow : public QDialog {
  Q_OBJECT

 public:
  WelcomeWindow(SanePreferences *preferences, QWidget *parent = nullptr);
  ~WelcomeWindow();

 private:
  SanePreferences *preferences;
  void updateText();
  QLabel *welcome;
  QLabel *warningLabel;
  QLabel *errorLabel;
  QDialogButtonBox *buttonBox;
  QPushButton *docButton;
  QPushButton *ignoreButton = nullptr;
  QPushButton *cancelButton = nullptr;
  QPushButton *okButton = nullptr;
#ifdef WITH_TRANSLATIONS
  QLabel *languageLabel;
  LanguageSelect *languageSelect;
#endif
};
