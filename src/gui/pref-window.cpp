// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pref-window.h"

#include <qglobal.h>

#include <QAudioOutput>
#include <QCheckBox>
#include <QColor>
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
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtContainerFwd>
#include <algorithm>
#include <utility>

#include "config.h"
#include "core/preferences.h"
#include "lib/auto-start.h"
#include "sound-player.h"
#include "ui_pref-window.h"
#include "widgets/language-select.h"
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
  emit loaded();
  emit explictSync();
  changeMeansDirty = true;
  if (isDirty) {
    isDirty = false;
    emit dirtyChanged();
  }
}

void PrefControllerBase::onChange() {
  // Do nothing if the change is caused by loading values
  if (!changeMeansDirty) return;
  if (!isDirty) {
    isDirty = true;
    emit dirtyChanged();
  }
  emit changed();
  emit explictSync();
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

PreferenceWindow::PreferenceWindow(SanePreferences *preferences, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PrefWindow), preferences(preferences) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon_tray-color.png"));

  ui->setupUi(this);
  ui->stackedWidget->setFixedHeight(
      ui->stackedWidget->currentWidget()->sizeHint().height());
  soundPlayer = new SoundPlayer(this);
  controllers = new ControllerHolder(this);
  autoStart = new AutoStart(this);

  /***************************************************************************
   *                                                                         *
   *                               Tab switch                                *
   *                                                                         *
   ****************************************************************************/
  tabButtons = {ui->tabScheduleButton, ui->tabReminderButton, ui->tabPauseButton,
                ui->tabSoundButton,    ui->tabGeneralButton,  ui->tabAboutButton};
  for (int i = 0; i < tabButtons.size(); ++i) {
    connect(tabButtons[i], &QPushButton::released, this, [this, i]() { setTab(i); });
  }
  // Qt Designer is having problem setting align center
  ui->sideBar->layout()->setAlignment(ui->sidebarImage, Qt::AlignHCenter);

  /***************************************************************************
   *                                                                         *
   *                              Schedule tab                               *
   *                                                                         *
   ****************************************************************************/
  connect(controllers->add(new PrefController<QSpinBox, Setting<int>>(
              ui->smallBreakEveryBox, preferences->smallEvery, 60)),
          &PrefControllerBase::explictSync, this, [this]() {
            int value = ui->smallBreakEveryBox->value();
            ui->bigBreakAfterBox->setToolTip(
                tr("Every %n min", "", value * ui->bigBreakAfterBox->value()));
          });
  controllers->add(new PrefController<QSpinBox, Setting<int>>(ui->smallBreakForBox,
                                                              preferences->smallFor));
  connect(controllers->add(new PrefController<QSpinBox, Setting<int>>(
              ui->bigBreakAfterBox, preferences->bigAfter)),
          &PrefControllerBase::explictSync, this, [this]() {
            int value = ui->bigBreakAfterBox->value();
            ui->bigBreakAfterBox->setToolTip(
                tr("Every %n min", "", value * ui->smallBreakEveryBox->value()));
          });
  controllers->add(new PrefController<QSpinBox, Setting<int>>(ui->bigBreakForBox,
                                                              preferences->bigFor, 60));

  QRegularExpression re("^\\d+(,\\d+)*$");
  ui->postponeMinutes->setValidator(new QRegularExpressionValidator(re, this));
  controllers->add(new PrefController<QLineEdit, Setting<QStringList>>(
      ui->postponeMinutes, preferences->postponeMinutes));

  /***************************************************************************
   *                                                                         *
   *                              Reminder tab                               *
   *                                                                         *
   ****************************************************************************/
  connect(controllers->add(new PrefController<QSpinBox, Setting<int>>(
              ui->flashForBox, preferences->flashFor)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->confirmAfterBox->setMaximum(ui->flashForBox->value());
            ui->confirmAfterSlider->setMaximum(ui->flashForBox->value());
            ui->reminderBehaviorLabel->setArgs(
                {ui->flashForBox->value(), ui->confirmAfterBox->value()});
          });
  connect(controllers->add(new PrefController<QSpinBox, Setting<int>>(
              ui->confirmAfterBox, preferences->confirmAfter)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->reminderBehaviorLabel->setArgs(
                {ui->flashForBox->value(), ui->confirmAfterBox->value()});
          });
  controllers->add(new PrefController<QSlider, Setting<int>>(ui->flashSpeedSlider,
                                                             preferences->flashSpeed));
  controllers->add(new PrefController<QCheckBox, Setting<bool>>(
      ui->showKillTip, preferences->showKillTip));

  controllers->add(new PrefController<QLineEdit, Setting<QColor>>(
      ui->backgroundColor, preferences->backgroundColor));
  controllers->add(new PrefController<QLineEdit, Setting<QColor>>(
      ui->messageColor, preferences->messageColor));
  controllers->add(new PrefController<QLineEdit, Setting<QColor>>(
      ui->smallHighlight, preferences->smallHighlightColor));
  controllers->add(new PrefController<QLineEdit, Setting<QColor>>(
      ui->bigHighlight, preferences->bigHighlightColor));
  controllers->add(new PrefController<QLineEdit, Setting<QColor>>(
      ui->countDownColor, preferences->countDownColor));

  controllers->add(new PrefController<QPlainTextEdit, Setting<QStringList>>(
      ui->smallBreakMessages, preferences->smallMessages));
  controllers->add(new PrefController<QPlainTextEdit, Setting<QStringList>>(
      ui->bigBreakMessages, preferences->bigMessages));

  /***************************************************************************
   *                                                                         *
   *                                Pause tab                                *
   *                                                                         *
   ****************************************************************************/
  controllers->add(new PrefController<QSpinBox, Setting<int>>(
      ui->pauseOnIdleBox, preferences->pauseOnIdleFor, 60));
  connect(controllers->add(new PrefController<QSpinBox, Setting<int>>(
              ui->resetBreakBox, preferences->resetAfterPause, 60)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->resetCycleBox->setMinimum(ui->resetBreakBox->value());
            ui->resetCycleSlider->setMinimum(ui->resetBreakBox->value());
          });
  connect(controllers->add(new PrefController<QSpinBox, Setting<int>>(
              ui->resetCycleBox, preferences->resetCycleAfterPause, 60)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->resetBreakBox->setMaximum(ui->resetCycleBox->value());
            ui->resetBreakSlider->setMaximum(ui->resetCycleBox->value());
          });
  controllers->add(new PrefController<QCheckBox, Setting<bool>>(
      ui->pauseOnBatteryCheck, preferences->pauseOnBattery));
#ifdef LINUX_DIST_FLATPAK
  ui->programList->setHidden(true);
  ui->programListLabel->setHidden(true);
#else
  controllers->add(new PrefController<QPlainTextEdit, Setting<QStringList>>(
      ui->programList, preferences->programsToMonitor));
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
      preferences->smallStartBell, preferences->smallEndBell, preferences->bigStartBell,
      preferences->bigEndBell};
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
   *                               General tab                               *
   *                                                                         *
   ****************************************************************************/
  ui->configFile->setText(preferences->settings->fileName());

  auto autoStartController =
      controllers->add(new PrefController<QCheckBox, Setting<bool>>(
          ui->autoStart, preferences->autoStart));
  connect(autoStartController, &PrefControllerBase::loaded, this, [this]() {
    // Tell the handler to ignore this change
    ui->autoStart->setDisabled(true);
    ui->autoStart->setChecked(autoStart->isEnabled(this->preferences));
    ui->autoStart->setDisabled(false);
  });
  connect(autoStartController, &PrefControllerBase::changed, this, [this]() {
    // Do nothing if the it is disabled to avoid races
    if (!ui->autoStart->isEnabled()) return;
    ui->autoStart->setDisabled(true);
    autoStart->setEnabled(ui->autoStart->isChecked());
  });
  connect(autoStart, &AutoStart::operationResult, this,
          [this](bool succeeded, QString error) {
            ui->autoStart->blockSignals(true);
            bool checked = ui->autoStart->isChecked();
            if (!succeeded) {
              ui->autoStart->setCheckState(!checked ? Qt::Checked : Qt::Unchecked);

              QMessageBox msgBox;
              msgBox.setText(tr("Setting autostart failed"));
              msgBox.setInformativeText(error);
              msgBox.setIcon(QMessageBox::Icon::Warning);
              msgBox.addButton(QMessageBox::Ok);
              msgBox.exec();
            }
            ui->autoStart->blockSignals(false);
            ui->autoStart->setDisabled(false);
          });
#ifdef WITH_TRANSLATIONS
  ui->languageSelect->setSelectedLanguage(preferences->language->get());
  controllers->add(new PrefController<LanguageSelect, Setting<QString>>(
      ui->languageSelect, preferences->language));
#else
  ui->languageLabel->setHidden(true);
  ui->languageSelect->setHidden(true);
  ui->languageHint->setHidden(true);
#endif
  ui->autoScreenLock->addItem(tr("Disabled"), 0);
  ui->autoScreenLock->addItem(tr("%n sec", "", 30), 30);
  ui->autoScreenLock->addItem(tr("%n min", "", 1), 60);
  ui->autoScreenLock->addItem(tr("%n min", "", 2), 120);
  ui->autoScreenLock->addItem(tr("%n min", "", 5), 300);
  ui->macPermissionHint->setHidden(true);
  controllers->add(new PrefController<QComboBox, Setting<int>>(
      ui->autoScreenLock, preferences->autoScreenLock));

#ifdef Q_OS_LINUX
  ui->quickBreakLabel->setText(
      tr("Start next break after middle clicking on tray icon"));
  controllers->add(new PrefController<QCheckBox, Setting<bool>>(
      ui->quickBreak, preferences->quickBreak));
#endif
#ifdef Q_OS_WIN
  ui->quickBreakLabel->setText(
      tr("Start next break after double clicking on tray icon"));
  controllers->add(new PrefController<QCheckBox, Setting<bool>>(
      ui->quickBreak, preferences->quickBreak));
#endif
#ifdef Q_OS_MAC
  ui->quickBreak->setHidden(true);
  ui->quickBreakLabel->setHidden(true);
#endif
  /***************************************************************************
   *                                                                         *
   *                                About tab                                *
   *                                                                         *
   ****************************************************************************/
  ui->copyrightLabel->setArgs({PROJECT_VERSION});
  QFile noticeFile(":/NOTICE.md");
  noticeFile.open(QIODevice::ReadOnly | QIODevice::Text);
  ui->noticeLabel->setText(noticeFile.readAll());

  connect(ui->resetButton, &QPushButton::pressed, controllers, &ControllerHolder::load);
  connect(ui->saveButton, &QPushButton::pressed, controllers, &ControllerHolder::save);
  connect(controllers, &ControllerHolder::dirtyChanged, ui->saveButton,
          &QPushButton::setEnabled);
  connect(controllers, &ControllerHolder::dirtyChanged, ui->resetButton,
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
  ui->controlBar->setHidden(tabNum == 5);
  ui->stackedWidget->setFixedHeight(
      ui->stackedWidget->currentWidget()->sizeHint().height());
}

bool PreferenceWindow::confirmLeave() {
  if (!controllers->isDirty) return true;

  QMessageBox msgBox;
  msgBox.setText(tr("The preferences have been modified."));
  msgBox.setInformativeText(tr("Do you want to save your changes?"));
  msgBox.setIcon(QMessageBox::Icon::Information);
  msgBox.addButton(QMessageBox::Save)->setText(tr("Save"));
  msgBox.addButton(QMessageBox::Discard)->setText(tr("Discard"));
  msgBox.addButton(QMessageBox::Cancel)->setText(tr("Cancel"));
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
