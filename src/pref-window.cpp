// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pref-window.h"

#include <QAudioOutput>
#include <QCheckBox>
#include <QDesktopServices>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QStringList>
#include <QStyleOptionSlider>
#include <QVBoxLayout>

#include "config.h"
#include "notice-window.h"
#include "preferences.h"
#include "ui_pref-window.h"
#include "widgets.h"

PreferenceWindow::PreferenceWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PrefWindow) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon_tray-color.png"));

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  ui->setupUi(centralWidget);

  /***************************************************************************
   *                                                                         *
   *                               Tab switch                                *
   *                                                                         *
   ****************************************************************************/
  connect(ui->tabBreakButton, &QPushButton::released, this, [this]() { setTab(0); });
  connect(ui->tabPauseButton, &QPushButton::released, this, [this]() { setTab(1); });
  connect(ui->tabAboutButton, &QPushButton::released, this, [this]() { setTab(2); });
  // Qt Designer is having problem setting align center
  ui->sideBar->layout()->setAlignment(ui->sidebarImage, Qt::AlignHCenter);

  /***************************************************************************
   *                                                                         *
   *                                Break tab                                *
   *                                                                         *
   ****************************************************************************/
  connect(ui->smallBreakEverySlider, &SteppedSlider::valueChanged, this,
          [this](int value) {
            ui->smallBreakEveryLabel->setText(QString("%1 min").arg(value));
            ui->bigBreakAfterLabel->setText(
                QString("%1 min").arg(value * ui->bigBreakAfterSlider->value()));
          });
  connect(ui->smallBreakForSlider, &SteppedSlider::valueChanged, this,
          [this](int value) {
            ui->smallBreakForLabel->setText(QString("%1 sec").arg(value));
          });
  connect(
      ui->bigBreakAfterSlider, &SteppedSlider::valueChanged, this, [this](int value) {
        ui->bigBreakAfterLabel->setText(QString("%1 breaks").arg(value));
        ui->bigBreakAfterLabel->setToolTip(
            QString("Every %1 min").arg(value * ui->smallBreakEverySlider->value()));
      });
  connect(ui->bigBreakForSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->bigBreakForLabel->setText(QString("%1 sec").arg(value));
  });
  connect(ui->flashForSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->flashForLabel->setText(QString("%1 sec").arg(value));
  });

  QStringList soundFiles = {"", "qrc:/sounds/chime.mp3", "qrc:/sounds/ding.mp3",
                            "qrc:/sounds/wood.mp3", "qrc:/sounds/bell.mp3"};
  ui->startSoundSelect->addItems(soundFiles);
  connect(ui->playStartSoundButton, &QPushButton::pressed, this,
          [this]() { playSound(ui->startSoundSelect->currentText()); });
  ui->endSoundSelect->addItems(soundFiles);
  connect(ui->playEndSoundButton, &QPushButton::pressed, this,
          [this]() { playSound(ui->endSoundSelect->currentText()); });

  /***************************************************************************
   *                                                                         *
   *                                Pause tab                                *
   *                                                                         *
   ****************************************************************************/
  connect(ui->pauseOnIdleSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->pauseOnIdleLabel->setText(QString("%1 min").arg(value));
  });
  connect(ui->resetBreakSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->resetBreakLabel->setText(QString("%1 min").arg(value));
    ui->resetCycleSlider->setMinimum(value);
  });
  connect(ui->resetCycleSlider, &SteppedSlider::valueChanged, this, [this](int value) {
    ui->resetCycleLabel->setText(QString("%1 min").arg(value));
  });

  /***************************************************************************
   *                                                                         *
   *                                About tab                                *
   *                                                                         *
   ****************************************************************************/
  ui->copyrightLabel->setText(ui->copyrightLabel->text().arg(PROJECT_VERSION));
  connect(ui->copyrightLabel, &QLabel::linkActivated, this, [=](QString url) {
    if (url == QString("notice-window"))
      (new NoticeWindow(this))->show();
    else
      QDesktopServices::openUrl(url);
  });

  connect(ui->resetButton, &QPushButton::pressed, this,
          &PreferenceWindow::loadSettings);
  connect(ui->saveButton, &QPushButton::pressed, this, &PreferenceWindow::close);
}

PreferenceWindow::~PreferenceWindow() { delete ui; }

void PreferenceWindow::loadSettings() {
  ui->smallBreakEverySlider->setValue(SanePreferences::smallEvery->get() / 60);
  ui->smallBreakForSlider->setValue(SanePreferences::smallFor->get());
  ui->bigBreakAfterSlider->setValue(SanePreferences::bigAfter->get());
  ui->bigBreakForSlider->setValue(SanePreferences::bigFor->get());
  ui->flashForSlider->setValue(SanePreferences::flashFor->get());
  ui->startSoundSelect->setEditText(SanePreferences::bellStart->get());
  ui->endSoundSelect->setEditText(SanePreferences::bellEnd->get());
  ui->pauseOnIdleSlider->setValue(SanePreferences::pauseOnIdleFor->get() / 60);
  ui->resetBreakSlider->setValue(SanePreferences::resetAfterPause->get() / 60);
  ui->resetCycleSlider->setValue(SanePreferences::resetCycleAfterPause->get() / 60);
  ui->pauseOnBatteryCheck->setChecked(SanePreferences::pauseOnBattery->get());
}

void PreferenceWindow::saveSettings() {
  SanePreferences::smallEvery->set(ui->smallBreakEverySlider->value() * 60);
  SanePreferences::smallFor->set(ui->smallBreakForSlider->value());
  SanePreferences::bigAfter->set(ui->bigBreakAfterSlider->value());
  SanePreferences::bigFor->set(ui->bigBreakForSlider->value());
  SanePreferences::flashFor->set(ui->flashForSlider->value());
  SanePreferences::bellStart->set(ui->startSoundSelect->currentText());
  SanePreferences::bellEnd->set(ui->endSoundSelect->currentText());
  SanePreferences::pauseOnIdleFor->set(ui->pauseOnIdleSlider->value() * 60);
  SanePreferences::resetAfterPause->set(ui->resetBreakSlider->value() * 60);
  SanePreferences::resetCycleAfterPause->set(ui->resetCycleSlider->value() * 60);
  SanePreferences::pauseOnBattery->set(ui->pauseOnBatteryCheck->isChecked());
}

void PreferenceWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  QMainWindow::closeEvent(event);
}

void PreferenceWindow::setTab(int tabNum) {
  ui->stackedWidget->setCurrentIndex(tabNum);
  ui->tabBreakButton->setChecked(tabNum == 0);
  ui->tabPauseButton->setChecked(tabNum == 1);
  ui->tabAboutButton->setChecked(tabNum == 2);
}

void PreferenceWindow::playSound(QString soundFile) {
  if (soundFile.isEmpty()) return;
  QMediaPlayer *soundPlayer = new QMediaPlayer(this);
  QAudioOutput *audioOutput = new QAudioOutput();
  soundPlayer->setAudioOutput(audioOutput);
  audioOutput->setVolume(100);
  soundPlayer->setSource(QUrl(soundFile));
  soundPlayer->play();
  connect(soundPlayer, &QMediaPlayer::playbackStateChanged, this,
          [=](QMediaPlayer::PlaybackState state) {
            if (state != QMediaPlayer::PlaybackState::StoppedState) return;
            soundPlayer->deleteLater();
            audioOutput->deleteLater();
          });
}
