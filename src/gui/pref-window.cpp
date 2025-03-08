// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pref-window.h"

#include <qglobal.h>

#include <QAudioOutput>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QEvent>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QSettings>
#include <QSlider>
#include <QString>
#include <QStringList>
#include <QStyleOptionSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtContainerFwd>

#include "config.h"
#include "preferences.h"
#include "sound-player.h"
#include "ui_pref-window.h"
#include "widgets/stepped-slider.h"

PreferenceWindow::PreferenceWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PrefWindow) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon_tray-color.png"));

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  ui->setupUi(centralWidget);
  soundPlayer = new SoundPlayer(this);

  /***************************************************************************
   *                                                                         *
   *                               Tab switch                                *
   *                                                                         *
   ****************************************************************************/
  tabButtons = {ui->tabBreakButton, ui->tabSoundButton, ui->tabPauseButton,
                ui->tabGeneralButton, ui->tabAboutButton};
  for (int i = 0; i < tabButtons.size(); ++i) {
    connect(tabButtons[i], &QPushButton::released, this, [this, i]() { setTab(i); });
  }
  // Qt Designer is having problem setting align center
  ui->sideBar->layout()->setAlignment(ui->sidebarImage, Qt::AlignHCenter);

  /***************************************************************************
   *                                                                         *
   *                                Break tab                                *
   *                                                                         *
   ****************************************************************************/
  connect(
      ui->smallBreakEverySlider, &SteppedSlider::valueChanged, this, [this](int value) {
        ui->smallBreakEveryLabel->setText(tr("%n min", "", value));
        ui->bigBreakAfterLabel->setToolTip(
            QString("Every %1 min").arg(value * ui->smallBreakEverySlider->value()));
      });
  connect(
      ui->smallBreakForSlider, &SteppedSlider::valueChanged, this,
      [this](int value) { ui->smallBreakForLabel->setText(tr("%n sec", "", value)); });
  connect(
      ui->bigBreakAfterSlider, &SteppedSlider::valueChanged, this, [this](int value) {
        ui->bigBreakAfterLabel->setText(tr("%n break(s)", "", value));
        ui->bigBreakAfterLabel->setToolTip(
            QString("Every %1 min").arg(value * ui->smallBreakEverySlider->value()));
      });
  connect(ui->bigBreakForSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->bigBreakForLabel->setText(tr("%n min", "", value));
  });
  connect(ui->flashForSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->flashForLabel->setText(tr("%n sec", "", value));
    ui->confirmAfterSlider->setMaximum(value);
  });
  connect(
      ui->confirmAfterSlider, &SteppedSlider::valueChanged, this,
      [this](int value) { ui->confirmAfterLabel->setText(tr("%n sec", "", value)); });

  QRegularExpression re("^\\d+(,\\d+)*$");
  ui->postponeMinutes->setValidator(new QRegularExpressionValidator(re, this));

  ui->autoScreenLock->addItem(tr("Disabled"), 0);
  ui->autoScreenLock->addItem(tr("%n sec", "", 30), 30);
  ui->autoScreenLock->addItem(tr("%n min", "", 1), 60);
  ui->autoScreenLock->addItem(tr("%n min", "", 2), 120);
  ui->autoScreenLock->addItem(tr("%n min", "", 5), 300);
  ui->macPermissionHint->setHidden(true);

#ifdef Q_OS_LINUX
  ui->quickBreak->setText(tr("Start next break after middle clicking on tray icon"));
#elif defined Q_OS_WIN
  ui->quickBreak->setText(tr("Start next break after double clicking on tray icon"));
#elif defined Q_OS_MAC
  ui->quickBreak->setHidden(true);
  osaProcess = new QProcess(this);
  // Set up permission
  connect(ui->autoScreenLock, &QComboBox::currentIndexChanged, this, [=](int index) {
    if (ui->autoScreenLock->itemData(index).toInt() == 0) return;
    if (osaProcess->isOpen()) return;
    osaProcess->start("osascript",
                      {"-e", "tell application \"System Events\" to keystroke \"q\""});
  });
  connect(osaProcess, &QProcess::finished, this,
          [=](int retcode, QProcess::ExitStatus status) {
            ui->macPermissionHint->setHidden(retcode == 0);
          });
#endif

  /***************************************************************************
   *                                                                         *
   *                                Sound tab                                *
   *                                                                         *
   ****************************************************************************/
  QStringList soundFiles = {"", "qrc:/sounds/chime.mp3", "qrc:/sounds/ding.mp3",
                            "qrc:/sounds/wood.mp3", "qrc:/sounds/bell.mp3"};
  QList<QComboBox *> soundSelects = {ui->smallStartBellSelect, ui->smallEndBellSelect,
                                     ui->bigStartBellSelect, ui->bigEndBellSelect};
  QList<QPushButton *> soundPlayButtons = {ui->playSmallStart, ui->playSmallEnd,
                                           ui->playBigStart, ui->playBigEnd};
  for (int i = 0; i < soundSelects.length(); i++) {
    soundSelects[i]->addItems(soundFiles);
    connect(soundPlayButtons[i], &QPushButton::pressed, soundPlayer,
            [this, soundSelects, i]() {
              soundPlayer->play(soundSelects[i]->currentText());
            });
  }
  /***************************************************************************
   *                                                                         *
   *                                Pause tab                                *
   *                                                                         *
   ****************************************************************************/
  connect(ui->pauseOnIdleSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->pauseOnIdleLabel->setText(tr("%n min", "", value));
  });
  connect(ui->resetBreakSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->resetBreakLabel->setText(tr("%n min", "", value));
    ui->resetCycleSlider->setMinimum(value);
  });
  connect(ui->resetCycleSlider, &SteppedSlider::valueChanged, this,
          [this](int value) { ui->resetCycleLabel->setText(tr("%n min", "", value)); });

  /***************************************************************************
   *                                                                         *
   *                               General tab                               *
   *                                                                         *
   ****************************************************************************/
  ui->configFile->setText(getSettings().fileName());
#ifndef WITH_TRANSLATIONS
  ui->languageLabel->setHidden(true);
  ui->languageSelect->setHidden(true);
  ui->languageHint->setHidden(true);
#endif
  /***************************************************************************
   *                                                                         *
   *                                About tab                                *
   *                                                                         *
   ****************************************************************************/
  ui->copyrightLabel->setText(ui->copyrightLabel->text().arg(PROJECT_VERSION));
  QFile noticeFile(":/NOTICE.md");
  noticeFile.open(QIODevice::ReadOnly | QIODevice::Text);
  ui->noticeLabel->setText(noticeFile.readAll());

  connect(ui->resetButton, &QPushButton::pressed, this,
          &PreferenceWindow::loadSettings);
  connect(ui->saveButton, &QPushButton::pressed, this, &PreferenceWindow::close);
}

PreferenceWindow::~PreferenceWindow() { delete ui; }

void PreferenceWindow::loadSettings() {
  ui->smallBreakEverySlider->setValue(SanePreferences::smallEvery->get() / 60);
  ui->smallBreakForSlider->setValue(SanePreferences::smallFor->get());
  ui->bigBreakAfterSlider->setValue(SanePreferences::bigAfter->get());
  ui->bigBreakForSlider->setValue(SanePreferences::bigFor->get() / 60);
  ui->flashForSlider->setValue(SanePreferences::flashFor->get());
  ui->confirmAfterSlider->setValue(SanePreferences::confirmAfter->get());
  ui->postponeMinutes->setText(SanePreferences::postponeMinutes->get().join(","));
  ui->quickBreak->setChecked(SanePreferences::quickBreak->get());
  ui->autoScreenLock->setCurrentIndex(
      ui->autoScreenLock->findData(SanePreferences::autoScreenLock->get()));
  ui->smallStartBellSelect->setEditText(SanePreferences::smallStartBell->get());
  ui->smallEndBellSelect->setEditText(SanePreferences::smallEndBell->get());
  ui->bigStartBellSelect->setEditText(SanePreferences::bigStartBell->get());
  ui->bigEndBellSelect->setEditText(SanePreferences::bigEndBell->get());
  ui->pauseOnIdleSlider->setValue(SanePreferences::pauseOnIdleFor->get() / 60);
  ui->resetBreakSlider->setValue(SanePreferences::resetAfterPause->get() / 60);
  ui->resetCycleSlider->setValue(SanePreferences::resetCycleAfterPause->get() / 60);
  ui->pauseOnBatteryCheck->setChecked(SanePreferences::pauseOnBattery->get());
  ui->programList->setPlainText(SanePreferences::programsToMonitor->get().join("\n"));
#ifdef WITH_TRANSLATIONS
  ui->languageSelect->setCurrentIndex(
      ui->languageSelect->findData(SanePreferences::language->get()));
#endif
}

void PreferenceWindow::saveSettings() {
  SanePreferences::smallEvery->set(ui->smallBreakEverySlider->value() * 60);
  SanePreferences::smallFor->set(ui->smallBreakForSlider->value());
  SanePreferences::bigAfter->set(ui->bigBreakAfterSlider->value());
  SanePreferences::bigFor->set(ui->bigBreakForSlider->value() * 60);
  SanePreferences::flashFor->set(ui->flashForSlider->value());
  SanePreferences::confirmAfter->set(ui->confirmAfterSlider->value());
  SanePreferences::postponeMinutes->set(
      ui->postponeMinutes->text().split(",", Qt::SkipEmptyParts));
  SanePreferences::quickBreak->set(ui->quickBreak->isChecked());
  SanePreferences::autoScreenLock->set(ui->autoScreenLock->currentData().toInt());
  SanePreferences::smallStartBell->set(ui->smallStartBellSelect->currentText());
  SanePreferences::smallEndBell->set(ui->smallEndBellSelect->currentText());
  SanePreferences::bigStartBell->set(ui->bigStartBellSelect->currentText());
  SanePreferences::bigEndBell->set(ui->bigEndBellSelect->currentText());
  SanePreferences::pauseOnIdleFor->set(ui->pauseOnIdleSlider->value() * 60);
  SanePreferences::resetAfterPause->set(ui->resetBreakSlider->value() * 60);
  SanePreferences::resetCycleAfterPause->set(ui->resetCycleSlider->value() * 60);
  SanePreferences::pauseOnBattery->set(ui->pauseOnBatteryCheck->isChecked());
  SanePreferences::programsToMonitor->set(
      ui->programList->toPlainText().split("\n", Qt::SkipEmptyParts));
#ifdef WITH_TRANSLATIONS
  SanePreferences::language->set(ui->languageSelect->currentData().toString());
#endif
}

void PreferenceWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  QMainWindow::closeEvent(event);
}

void PreferenceWindow::setTab(int tabNum) {
  ui->stackedWidget->setCurrentIndex(tabNum);
  for (int i = 0; i < tabButtons.size(); ++i) {
    tabButtons[i]->setChecked(i == tabNum);
  }
  ui->controlBar->setHidden(tabNum == 4);
}
