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
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <QtContainerFwd>
#include <functional>

#include "app/break-windows.h"
#include "app/sound-player.h"
#include "app/text-window.h"
#include "app/widgets/language-select.h"
#include "app/widgets/stepped-slider.h"
#include "config.h"
#include "core/break-windows.h"
#include "core/flags.h"
#include "core/preferences.h"
#include "lib/auto-start.h"
#include "ui_pref-window.h"

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

PrefControllerBase *ControllerHolder::add(PrefGroup group,
                                          PrefControllerBase *controller) {
  m_controllers[group].append(controller);
  connect(controller, &PrefControllerBase::dirtyChanged, this,
          &ControllerHolder::onDirtyChange);
  return controller;
}

void ControllerHolder::forEach(std::function<void(PrefControllerBase *)> func) {
  for (const auto &controllerList : m_controllers) {
    for (const auto &controller : controllerList) {
      func(controller);
    }
  }
}

void ControllerHolder::onDirtyChange() {
  bool currentIsDirty = false;
  forEach([&currentIsDirty](PrefControllerBase *controller) {
    if (controller->isDirty) currentIsDirty = true;
  });
  if (isDirty != currentIsDirty) {
    isDirty = currentIsDirty;
    emit dirtyChanged(currentIsDirty);
  }
}

void ControllerHolder::load() {
  forEach([](PrefControllerBase *controller) { controller->load(); });
}

void ControllerHolder::reloadDirty() {
  forEach([](PrefControllerBase *controller) {
    if (controller->isDirty) controller->load();
  });
}

void ControllerHolder::save() {
  forEach([](PrefControllerBase *controller) { controller->saveIfDirty(); });
}

void ControllerHolder::setGroupToDefault(PrefGroup group) {
  for (auto &controller : m_controllers[group]) {
    controller->setToDefault();
  }
}

PreferenceWindow::PreferenceWindow(SanePreferences *preferences, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::PrefWindow), preferences(preferences) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon.png"));

  ui->setupUi(this);
  ui->stackedWidget->setFixedHeight(
      ui->stackedWidget->currentWidget()->sizeHint().height());
  soundPlayer = new SoundPlayer(this);
  controllers = new ControllerHolder(this);
  autoStart = new AutoStart(this);
  breakWindows = new BreakWindows(this);

  /***************************************************************************
   *                                                                         *
   *                               Tab switch                                *
   *                                                                         *
   ****************************************************************************/
  tabButtons = {ui->tabScheduleButton, ui->tabReminderButton, ui->tabInterfaceButton,
                ui->tabPauseButton,    ui->tabSoundButton,    ui->tabGeneralButton,
                ui->tabAboutButton};
  for (int i = 0; i < tabButtons.size(); ++i) {
    connect(tabButtons[i], &QPushButton::released, this,
            [this, i]() { setTab(i, tabButtons.length()); });
  }
  // Qt Designer is having problem setting align center
  ui->sideBar->layout()->setAlignment(ui->sidebarImage, Qt::AlignHCenter);

  /***************************************************************************
   *                                                                         *
   *                              Schedule tab                               *
   *                                                                         *
   ****************************************************************************/
  connect(controllers->add(PrefGroup::Schedule,
                           new PrefController<QSpinBox, int>(
                               ui->smallBreakEveryBox, preferences->smallEvery, 60)),
          &PrefControllerBase::explictSync, this, [this]() {
            int value = ui->smallBreakEveryBox->value();
            ui->bigBreakAfterBox->setToolTip(
                tr("Every %n min", "", value * ui->bigBreakAfterBox->value()));
          });
  controllers->add(
      PrefGroup::Schedule,
      new PrefController<QSpinBox, int>(ui->smallBreakForBox, preferences->smallFor));
  connect(controllers->add(PrefGroup::Schedule,
                           new PrefController<QSpinBox, int>(ui->bigBreakAfterBox,
                                                             preferences->bigAfter)),
          &PrefControllerBase::explictSync, this, [this]() {
            int value = ui->bigBreakAfterBox->value();
            ui->bigBreakAfterBox->setToolTip(
                tr("Every %n min", "", value * ui->smallBreakEveryBox->value()));
          });
  controllers->add(
      PrefGroup::Schedule,
      new PrefController<QSpinBox, int>(ui->bigBreakForBox, preferences->bigFor, 60));

  QRegularExpression re("^\\d+(,\\d+)*$");
  ui->postponeMinutes->setValidator(new QRegularExpressionValidator(re, this));
  controllers->add(PrefGroup::Schedule,
                   new PrefController<QLineEdit, QStringList>(
                       ui->postponeMinutes, preferences->postponeMinutes));

  /***************************************************************************
   *                                                                         *
   *                              Reminder tab                               *
   *                                                                         *
   ****************************************************************************/
  connect(controllers->add(PrefGroup::Reminder,
                           new PrefController<QSpinBox, int>(ui->flashForBox,
                                                             preferences->flashFor)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->confirmAfterBox->setMaximum(ui->flashForBox->value());
            ui->confirmAfterSlider->setMaximum(ui->flashForBox->value());
            ui->reminderBehaviorLabel->setArgs(
                {ui->flashForBox->value(), ui->confirmAfterBox->value()});
          });
  connect(controllers->add(PrefGroup::Reminder,
                           new PrefController<QSpinBox, int>(
                               ui->confirmAfterBox, preferences->confirmAfter)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->reminderBehaviorLabel->setArgs(
                {ui->flashForBox->value(), ui->confirmAfterBox->value()});
          });
  controllers->add(PrefGroup::Reminder,
                   new PrefController<QSpinBox, int>(ui->maxForceBreakExits,
                                                     preferences->maxForceBreakExits));
  controllers->add(
      PrefGroup::Reminder,
      new PrefController<QCheckBox, bool>(ui->autoCloseWindowAfterSmallBreak,
                                          preferences->autoCloseWindowAfterSmallBreak));
  controllers->add(PrefGroup::Reminder, new PrefController<QCheckBox, bool>(
                                            ui->autoCloseWindowAfterBigBreak,
                                            preferences->autoCloseWindowAfterBigBreak));

  /***************************************************************************
   *                                                                         *
   *                             Interface tab                               *
   *                                                                         *
   ****************************************************************************/
  controllers->add(
      PrefGroup::Interface,
      new PrefController<QSpinBox, int>(ui->flashSpeedBox, preferences->flashSpeed));

  controllers->add(PrefGroup::Interface,
                   new PrefController<QLineEdit, QColor>(ui->backgroundColor,
                                                         preferences->backgroundColor));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QLineEdit, QColor>(ui->messageColor,
                                                         preferences->messageColor));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QLineEdit, QColor>(
                       ui->smallHighlight, preferences->smallHighlightColor));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QLineEdit, QColor>(
                       ui->bigHighlight, preferences->bigHighlightColor));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QLineEdit, QColor>(ui->countDownColor,
                                                         preferences->countDownColor));

  controllers->add(PrefGroup::Interface,
                   new PrefController<QPlainTextEdit, QStringList>(
                       ui->smallBreakMessages, preferences->smallMessages));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QPlainTextEdit, QStringList>(
                       ui->bigBreakMessages, preferences->bigMessages));

  controllers->add(PrefGroup::Interface, new PrefController<QCheckBox, bool>(
                                             ui->smallBreakShowProgressBar,
                                             preferences->smallBreakShowProgressBar));
  controllers->add(PrefGroup::Interface, new PrefController<QCheckBox, bool>(
                                             ui->smallBreakShowCountdown,
                                             preferences->smallBreakShowCountdown));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QCheckBox, bool>(
                       ui->smallBreakShowClock, preferences->smallBreakShowClock));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QCheckBox, bool>(
                       ui->smallBreakShowEndTime, preferences->smallBreakShowEndTime));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QCheckBox, bool>(
                       ui->smallBreakShowButtons, preferences->smallBreakShowButtons));

  controllers->add(PrefGroup::Interface, new PrefController<QCheckBox, bool>(
                                             ui->bigBreakShowProgressBar,
                                             preferences->bigBreakShowProgressBar));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QCheckBox, bool>(
                       ui->bigBreakShowCountdown, preferences->bigBreakShowCountdown));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QCheckBox, bool>(ui->bigBreakShowClock,
                                                       preferences->bigBreakShowClock));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QCheckBox, bool>(
                       ui->bigBreakShowEndTime, preferences->bigBreakShowEndTime));
  controllers->add(PrefGroup::Interface,
                   new PrefController<QCheckBox, bool>(
                       ui->bigBreakShowButtons, preferences->bigBreakShowButtons));

  /***************************************************************************
   *                                                                         *
   *                                Pause tab                                *
   *                                                                         *
   ****************************************************************************/
  controllers->add(PrefGroup::Pause,
                   new PrefController<QSpinBox, int>(ui->pauseOnIdleBox,
                                                     preferences->pauseOnIdleFor, 60));
  connect(controllers->add(PrefGroup::Pause,
                           new PrefController<QSpinBox, int>(
                               ui->resetBreakBox, preferences->resetAfterPause, 60)),
          &PrefControllerBase::explictSync, this, [this]() {
            ui->resetCycleBox->setMinimum(ui->resetBreakBox->value());
            ui->resetCycleSlider->setMinimum(ui->resetBreakBox->value());
          });
  connect(
      controllers->add(PrefGroup::Pause,
                       new PrefController<QSpinBox, int>(
                           ui->resetCycleBox, preferences->resetCycleAfterPause, 60)),
      &PrefControllerBase::explictSync, this, [this]() {
        ui->resetBreakBox->setMaximum(ui->resetCycleBox->value());
        ui->resetBreakSlider->setMaximum(ui->resetCycleBox->value());
      });
  controllers->add(PrefGroup::Pause,
                   new PrefController<QCheckBox, bool>(ui->pauseOnBatteryCheck,
                                                       preferences->pauseOnBattery));
#ifdef LINUX_DIST_FLATPAK
  ui->programList->setHidden(true);
  ui->programListLabel->setHidden(true);
#else
  controllers->add(PrefGroup::Pause,
                   new PrefController<QPlainTextEdit, QStringList>(
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
    controllers->add(PrefGroup::Sound, new PrefController<QComboBox, QString>(
                                           soundSelects[i], soundSettings[i]));
  }

  /***************************************************************************
   *                                                                         *
   *                               General tab                               *
   *                                                                         *
   ****************************************************************************/
  ui->configFile->setText(preferences->settings->fileName());

  auto autoStartController = controllers->add(
      PrefGroup::General,
      new PrefController<QCheckBox, bool>(ui->autoStart, preferences->autoStart));
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
  controllers->add(PrefGroup::General, new PrefController<LanguageSelect, QString>(
                                           ui->languageSelect, preferences->language));
#else
  ui->languageLabel->setHidden(true);
  ui->languageSelect->setHidden(true);
  ui->languageHint->setHidden(true);
  ui->languageLink->setHidden(true);
#endif

#ifdef LINUX_DIST_FLATPAK
  ui->autoScreenLock->setHidden(true);
  ui->autoScreenLockLabel->setHidden(true);
#else
  ui->autoScreenLock->addItem(tr("Disabled"), 0);
  ui->autoScreenLock->addItem(tr("%n sec", "", 30), 30);
  ui->autoScreenLock->addItem(tr("%n min", "", 1), 60);
  ui->autoScreenLock->addItem(tr("%n min", "", 2), 120);
  ui->autoScreenLock->addItem(tr("%n min", "", 5), 300);
  controllers->add(PrefGroup::General,
                   new PrefController<QComboBox, int>(ui->autoScreenLock,
                                                      preferences->autoScreenLock));
#endif

#ifdef Q_OS_LINUX
  ui->quickBreak->setText(tr("Start next break after middle clicking on tray icon"));
  controllers->add(PrefGroup::General, new PrefController<QCheckBox, bool>(
                                           ui->quickBreak, preferences->quickBreak));
#endif
#ifdef Q_OS_WIN
  ui->quickBreak->setText(tr("Start next break after double clicking on tray icon"));
  controllers->add(PrefGroup::General, new PrefController<QCheckBox, bool>(
                                           ui->quickBreak, preferences->quickBreak));
#endif
#ifdef Q_OS_MAC
  ui->quickBreak->setHidden(true);
#endif
  /***************************************************************************
   *                                                                         *
   *                                About tab                                *
   *                                                                         *
   ****************************************************************************/
  ui->copyrightLabel->setArgs({PROJECT_VERSION});
  connect(ui->noticeButton, &QPushButton::pressed, this, &PreferenceWindow::openNotice);
  connect(ui->sourceCodeButton, &QPushButton::pressed, this,
          &PreferenceWindow::openSourceCode);
  connect(ui->helpTranslateButton, &QPushButton::pressed, this,
          &PreferenceWindow::openWeblate);

  /***************************************************************************
   *                                                                         *
   *                               Bottom row                                *
   *                                                                         *
   ****************************************************************************/
  connect(ui->resetButton, &QPushButton::pressed, controllers, &ControllerHolder::load);
  connect(ui->saveButton, &QPushButton::pressed, controllers, &ControllerHolder::save);
  connect(ui->defaultsButton, &QPushButton::pressed, this, [this]() {
    this->controllers->setGroupToDefault(
        static_cast<PrefGroup>(this->ui->stackedWidget->currentIndex()));
  });
  connect(ui->previewButton, &QPushButton::pressed, this,
          &PreferenceWindow::openBreakWindowPreview);
  connect(controllers, &ControllerHolder::dirtyChanged, ui->previewButton,
          &QPushButton::setDisabled);
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

void PreferenceWindow::showEvent(QShowEvent *) { controllers->load(); }

void PreferenceWindow::setTab(int tabNum, int totalTabCount) {
  if (!confirmLeave()) {
    tabButtons[tabNum]->setChecked(false);
    return;
  }
  ui->stackedWidget->setCurrentIndex(tabNum);
  for (int i = 0; i < tabButtons.size(); ++i) {
    tabButtons[i]->setChecked(i == tabNum);
  }
  ui->controlBar->setHidden(tabNum == totalTabCount - 1);
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

void PreferenceWindow::openNotice() {
  if (openingTextWindow != nullptr) {
    openingTextWindow->close();
  }
  QFile noticeFile(":/NOTICE.md");
  noticeFile.open(QIODevice::ReadOnly | QIODevice::Text);
  openingTextWindow = new TextWindow(noticeFile.readAll());
  openingTextWindow->show();
}

void PreferenceWindow::openSourceCode() {
  QDesktopServices::openUrl(QUrl("https://github.com/AllanChain/sane-break"));
}

void PreferenceWindow::openWeblate() {
  QDesktopServices::openUrl(QUrl("https://hosted.weblate.org/engage/sane-break/"));
}

void PreferenceWindow::openBreakWindowPreview() {
  breakWindows->create(BreakType::Small, preferences);
  breakWindows->showFlashPrompt();
  QTimer::singleShot(2000, [this]() {
    breakWindows->showFullScreen();
    breakWindows->showButtons(AbstractBreakWindows::Button::LockScreen |
                              AbstractBreakWindows::Button::ExitForceBreak);
    QTimer::singleShot(4000, [this]() {
      breakWindows->destroy();
      breakWindows->create(BreakType::Big, preferences);
      breakWindows->showFlashPrompt();
      QTimer::singleShot(2000, [this]() {
        breakWindows->showFullScreen();
        breakWindows->showButtons(AbstractBreakWindows::Button::LockScreen |
                                  AbstractBreakWindows::Button::ExitForceBreak);
        QTimer::singleShot(4000, [this]() { breakWindows->destroy(); });
      });
    });
  });
}
