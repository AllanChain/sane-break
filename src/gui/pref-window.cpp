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
#include <QLineEdit>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QPlainTextEdit>
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
#include <algorithm>
#include <utility>

#include "config.h"
#include "preferences.h"
#include "sound-player.h"
#include "ui_pref-window.h"
#include "widgets/stepped-slider.h"

void PrefControllerBase::saveIfDirty() {
  if (isDirty) {
    saveValue();
    isDirty = false;
    emit dirtyChanged();
  }
}

void PrefControllerBase::load() {
  changeMeansDirty = false;
  loadValue();
  emit explictSync();
  changeMeansDirty = true;
  isDirty = false;
  emit dirtyChanged();
}

void PrefControllerBase::onChange() {
  if (changeMeansDirty) {
    isDirty = true;
    emit dirtyChanged();
    emit explictSync();
  }
}

PrefControllerBase *ControllerHolder::add(PrefControllerBase *controller) {
  m_controllers.append(controller);
  connect(controller, &PrefControllerBase::dirtyChanged, this,
          &ControllerHolder::onDirtyChange);
  return controller;
}

void ControllerHolder::onDirtyChange() {
  bool currentIsDirty =
      std::any_of(m_controllers.begin(), m_controllers.end(),
                  [](PrefControllerBase *controller) { return controller->isDirty; });
  if (isDirty != currentIsDirty) {
    isDirty = currentIsDirty;
    emit dirtyChanged(currentIsDirty);
  }
}

void ControllerHolder::load() {
  for (auto controller : std::as_const(m_controllers)) controller->load();
}

void ControllerHolder::reloadDirty() {
  for (auto controller : std::as_const(m_controllers))
    if (controller->isDirty) controller->load();
}

void ControllerHolder::save() {
  for (auto controller : std::as_const(m_controllers)) controller->saveIfDirty();
}

PreferenceWindow::PreferenceWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PrefWindow) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon_tray-color.png"));

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  ui->setupUi(centralWidget);
  soundPlayer = new SoundPlayer(this);
  controllers = new ControllerHolder(this);

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
  connect(controllers->add(new PrefController<QSlider, Setting<int>>(
              ui->smallBreakEverySlider, SanePreferences::smallEvery, 60)),
          &PrefControllerBase::explictSync, this, [this]() {
            int value = ui->smallBreakEverySlider->value();
            ui->smallBreakEveryLabel->setText(tr("%n min", "", value));
            ui->bigBreakAfterLabel->setToolTip(
                tr("Every %n min", "", value * ui->bigBreakAfterSlider->value()));
          });
  connect(controllers->add(new PrefController<QSlider, Setting<int>>(
              ui->smallBreakForSlider, SanePreferences::smallFor)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->smallBreakForLabel->setText(
                tr("%n sec", "", ui->smallBreakEverySlider->value()));
          });
  connect(controllers->add(new PrefController<QSlider, Setting<int>>(
              ui->bigBreakAfterSlider, SanePreferences::bigAfter)),
          &PrefControllerBase::explictSync, this, [this]() {
            int value = ui->bigBreakAfterSlider->value();
            ui->bigBreakAfterLabel->setText(tr("%n break(s)", "", value));
            ui->bigBreakAfterLabel->setToolTip(
                tr("Every %n min", "", value * ui->smallBreakEverySlider->value()));
          });
  connect(
      controllers->add(new PrefController<QSlider, Setting<int>>(
          ui->bigBreakForSlider, SanePreferences::bigFor, 60)),
      &PrefControllerBase::explictSync, this, [this]() {
        ui->bigBreakForLabel->setText(tr("%n min", "", ui->bigBreakForSlider->value()));
      });
  connect(controllers->add(new PrefController<QSlider, Setting<int>>(
              ui->flashForSlider, SanePreferences::flashFor)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->flashForLabel->setText(tr("%n sec", "", ui->flashForSlider->value()));
            ui->confirmAfterSlider->setMaximum(ui->flashForSlider->value());
          });
  connect(controllers->add(new PrefController<QSlider, Setting<int>>(
              ui->confirmAfterSlider, SanePreferences::confirmAfter)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->confirmAfterLabel->setText(
                tr("%n sec", "", ui->confirmAfterSlider->value()));
          });

  QRegularExpression re("^\\d+(,\\d+)*$");
  ui->postponeMinutes->setValidator(new QRegularExpressionValidator(re, this));
  controllers->add(new PrefController<QLineEdit, Setting<QStringList>>(
      ui->postponeMinutes, SanePreferences::postponeMinutes));

  ui->autoScreenLock->addItem(tr("Disabled"), 0);
  ui->autoScreenLock->addItem(tr("%n sec", "", 30), 30);
  ui->autoScreenLock->addItem(tr("%n min", "", 1), 60);
  ui->autoScreenLock->addItem(tr("%n min", "", 2), 120);
  ui->autoScreenLock->addItem(tr("%n min", "", 5), 300);
  ui->macPermissionHint->setHidden(true);
  controllers->add(new PrefController<QComboBox, Setting<int>>(
      ui->autoScreenLock, SanePreferences::autoScreenLock));

#ifdef Q_OS_LINUX
  ui->quickBreak->setText(tr("Start next break after middle clicking on tray icon"));
  controllers->add(new PrefController<QCheckBox, Setting<bool>>(
      ui->quickBreak, SanePreferences::quickBreak));
#elif defined Q_OS_WIN
  ui->quickBreak->setText(tr("Start next break after double clicking on tray icon"));
  controllers->add(new PrefController<QCheckBox, Setting<bool>>(
      ui->quickBreak, SanePreferences::quickBreak));
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
  QList<Setting<QString> *> soundSettings = {
      SanePreferences::smallStartBell, SanePreferences::smallEndBell,
      SanePreferences::bigStartBell, SanePreferences::bigEndBell};
  for (int i = 0; i < soundSelects.length(); i++) {
    soundSelects[i]->addItems(soundFiles);
    connect(soundPlayButtons[i], &QPushButton::pressed, soundPlayer,
            [this, soundSelects, i]() {
              soundPlayer->play(soundSelects[i]->currentText());
            });
    controllers->add(new PrefController<QComboBox, Setting<QString>>(soundSelects[i],
                                                                     soundSettings[i]));
  }
  /***************************************************************************
   *                                                                         *
   *                                Pause tab                                *
   *                                                                         *
   ****************************************************************************/
  connect(
      controllers->add(new PrefController<QSlider, Setting<int>>(
          ui->pauseOnIdleSlider, SanePreferences::pauseOnIdleFor, 60)),
      &PrefControllerBase::explictSync, this, [this]() {
        ui->pauseOnIdleLabel->setText(tr("%n min", "", ui->pauseOnIdleSlider->value()));
      });
  connect(
      controllers->add(new PrefController<QSlider, Setting<int>>(
          ui->resetBreakSlider, SanePreferences::resetAfterPause, 60)),
      &PrefControllerBase::explictSync, this, [this]() {
        ui->resetBreakLabel->setText(tr("%n min", "", ui->resetBreakSlider->value()));
        ui->resetCycleSlider->setMinimum(ui->resetBreakSlider->value());
      });
  connect(
      controllers->add(new PrefController<QSlider, Setting<int>>(
          ui->resetCycleSlider, SanePreferences::resetCycleAfterPause, 60)),
      &PrefControllerBase::explictSync, this, [this]() {
        ui->resetCycleLabel->setText(tr("%n min", "", ui->resetCycleSlider->value()));
        ui->resetBreakSlider->setMaximum(ui->resetCycleSlider->value());
      });
  controllers->add(new PrefController<QCheckBox, Setting<bool>>(
      ui->pauseOnBatteryCheck, SanePreferences::pauseOnBattery));
  controllers->add(new PrefController<QPlainTextEdit, Setting<QStringList>>(
      ui->programList, SanePreferences::programsToMonitor));

  /***************************************************************************
   *                                                                         *
   *                               General tab                               *
   *                                                                         *
   ****************************************************************************/
  ui->configFile->setText(getSettings().fileName());
  // TODO: make languageSelect under control
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

  connect(ui->resetButton, &QPushButton::pressed, controllers, &ControllerHolder::load);
  connect(ui->saveButton, &QPushButton::pressed, controllers, &ControllerHolder::save);
  connect(controllers, &ControllerHolder::dirtyChanged, ui->saveButton,
          &QPushButton::setEnabled);
}

PreferenceWindow::~PreferenceWindow() { delete ui; }

void PreferenceWindow::closeEvent(QCloseEvent *event) {
  if (!confirmLeave()) return event->ignore();
  QMainWindow::closeEvent(event);
}

void PreferenceWindow::showEvent(QShowEvent *event) { controllers->load(); }

void PreferenceWindow::setTab(int tabNum) {
  if (!confirmLeave()) {
    tabButtons[tabNum]->setChecked(false);
    return;
  }
  ui->stackedWidget->setCurrentIndex(tabNum);
  for (int i = 0; i < tabButtons.size(); ++i) {
    tabButtons[i]->setChecked(i == tabNum);
  }
  ui->controlBar->setHidden(tabNum == 4);
}

bool PreferenceWindow::confirmLeave() {
  if (!controllers->isDirty) return true;

  QMessageBox msgBox;
  msgBox.setText(tr("The preferences have been modified."));
  msgBox.setInformativeText(tr("Do you want to save your changes?"));
  msgBox.setIcon(QMessageBox::Icon::Information);
  msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                            QMessageBox::Cancel);

  msgBox.setDefaultButton(QMessageBox::Save);
  switch (msgBox.exec()) {
    case QMessageBox::Save:
      controllers->save();
      return true;
    case QMessageBox::Discard:
      controllers->reloadDirty();
      return true;
    case QMessageBox::Cancel:
    default:
      return false;
  }
}
