// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pref-window.h"

#include <QAudioOutput>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopServices>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QPushButton>
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
  tabButtons = {ui->tabBreakButton, ui->tabSoundButton, ui->tabPauseButton,
                ui->tabAboutButton};
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
    ui->confirmAfterSlider->setMaximum(value);
  });
  connect(ui->confirmAfterSlider, &SteppedSlider::valueChanged, this,
          [this](int value) {
            ui->confirmAfterLabel->setText(QString("%1 sec").arg(value));
          });

  QStringList soundFiles = {"", "qrc:/sounds/chime.mp3", "qrc:/sounds/ding.mp3",
                            "qrc:/sounds/wood.mp3", "qrc:/sounds/bell.mp3"};
  QList<QComboBox *> soundSelects = {ui->smallStartBellSelect, ui->smallEndBellSelect,
                                     ui->bigStartBellSelect, ui->bigEndBellSelect};
  QList<QPushButton *> soundPlayButtons = {ui->playSmallStart, ui->playSmallEnd,
                                           ui->playBigStart, ui->playBigEnd};
  for (int i = 0; i < soundSelects.length(); i++) {
    soundSelects[i]->addItems(soundFiles);
    connect(soundPlayButtons[i], &QPushButton::pressed, this,
            [this, soundSelects, i]() { playSound(soundSelects[i]->currentText()); });
  }
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
  ui->bigBreakForSlider->setValue(SanePreferences::bigFor->get());
  ui->flashForSlider->setValue(SanePreferences::flashFor->get());
  ui->confirmAfterSlider->setValue(SanePreferences::confirmAfter->get());
  ui->smallStartBellSelect->setEditText(SanePreferences::smallStartBell->get());
  ui->smallEndBellSelect->setEditText(SanePreferences::smallEndBell->get());
  ui->bigStartBellSelect->setEditText(SanePreferences::bigStartBell->get());
  ui->bigEndBellSelect->setEditText(SanePreferences::bigEndBell->get());
  ui->pauseOnIdleSlider->setValue(SanePreferences::pauseOnIdleFor->get() / 60);
  ui->resetBreakSlider->setValue(SanePreferences::resetAfterPause->get() / 60);
  ui->resetCycleSlider->setValue(SanePreferences::resetCycleAfterPause->get() / 60);
  ui->pauseOnBatteryCheck->setChecked(SanePreferences::pauseOnBattery->get());
  ui->programList->setPlainText(SanePreferences::programsToMonitor->get().join("\n"));
}

void PreferenceWindow::saveSettings() {
  SanePreferences::smallEvery->set(ui->smallBreakEverySlider->value() * 60);
  SanePreferences::smallFor->set(ui->smallBreakForSlider->value());
  SanePreferences::bigAfter->set(ui->bigBreakAfterSlider->value());
  SanePreferences::bigFor->set(ui->bigBreakForSlider->value());
  SanePreferences::flashFor->set(ui->flashForSlider->value());
  SanePreferences::confirmAfter->set(ui->confirmAfterSlider->value());
  SanePreferences::smallStartBell->set(ui->smallStartBellSelect->currentText());
  SanePreferences::smallEndBell->set(ui->smallEndBellSelect->currentText());
  SanePreferences::bigStartBell->set(ui->bigStartBellSelect->currentText());
  SanePreferences::bigEndBell->set(ui->bigEndBellSelect->currentText());
  SanePreferences::pauseOnIdleFor->set(ui->pauseOnIdleSlider->value() * 60);
  SanePreferences::resetAfterPause->set(ui->resetBreakSlider->value() * 60);
  SanePreferences::resetCycleAfterPause->set(ui->resetCycleSlider->value() * 60);
  SanePreferences::pauseOnBattery->set(ui->pauseOnBatteryCheck->isChecked());
  SanePreferences::programsToMonitor->set(ui->programList->toPlainText().split("\n"));
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
  ui->controlBar->setHidden(tabNum == 3);
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
