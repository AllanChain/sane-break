// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_WELCOME_H
#define SANE_WELCOME_H
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QLabel>
#include <QWidget>

class WelcomeWindow : public QDialog {
  Q_OBJECT

 public:
  WelcomeWindow(QWidget *parent = nullptr);
  ~WelcomeWindow();

 protected:
  void changeEvent(QEvent *event);

 private:
  void updateText();
  void onLanguageSelect();
  QLabel *welcome;
  QLabel *warningLabel;
  QLabel *errorLabel;
  QLabel *languageLabel;
  QComboBox *languageSelect;
  QDialogButtonBox *buttonBox;
  QPushButton *docButton;
  QPushButton *ignoreButton = nullptr;
  QPushButton *cancelButton = nullptr;
  QPushButton *okButton = nullptr;
};
#endif  // SANE_WELCOME_H
