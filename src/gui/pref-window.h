// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_WINDOW_H
#define SANE_PREFERENCES_WINDOW_H
#include <qlineedit.h>

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QEvent>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

#include "lib/auto-start.h"
#include "lib/preferences.h"
#include "sound-player.h"
#include "widgets/language-select.h"

class PrefControllerBase : public QObject {
  Q_OBJECT
 public:
  PrefControllerBase(QObject *parent = 0) : QObject(parent) {}
  bool isDirty = false;
  bool changeMeansDirty = true;
  void saveIfDirty();
  void load();
  virtual void saveValue() {};
  virtual void loadValue() {};
  void onChange();
 signals:
  void loaded();
  // This is like onChange for underlying widgets, but also emits when values are loaded
  // from settings (which does not necessarily change the value of the widget). This is
  // useful when we want to sync the labels with the value of the setting.
  void explictSync();
  void dirtyChanged();
};

template <typename W, typename S>
class PrefController : public PrefControllerBase {
 public:
  W *widget;
  S *setting;
  PrefController(W *parent, S *setting)
      : PrefControllerBase(parent), widget(parent), setting(setting) {};
};

template <>
class PrefController<QCheckBox, Setting<bool>> : public PrefControllerBase {
 public:
  QCheckBox *widget;
  Setting<bool> *setting;
  PrefController(QCheckBox *parent, Setting<bool> *setting)
      : PrefControllerBase(parent), widget(parent), setting(setting) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    connect(widget, &QCheckBox::checkStateChanged, this, &PrefControllerBase::onChange);
#else
    connect(widget, &QCheckBox::stateChanged, this, &PrefControllerBase::onChange);
#endif
  };
  void loadValue() { widget->setChecked(setting->get()); }
  void saveValue() { setting->set(widget->isChecked()); }
};

template <>
class PrefController<QSlider, Setting<int>> : public PrefControllerBase {
 public:
  QSlider *widget;
  Setting<int> *setting;
  int multiplier;
  PrefController(QSlider *parent, Setting<int> *setting, int multiplier = 1)
      : PrefControllerBase(parent),
        widget(parent),
        setting(setting),
        multiplier(multiplier) {
    connect(widget, &QSlider::valueChanged, this, &PrefControllerBase::onChange);
  };
  void loadValue() { widget->setValue(setting->get() / multiplier); }
  void saveValue() { setting->set(widget->value() * multiplier); }
};

template <>
class PrefController<QSpinBox, Setting<int>> : public PrefControllerBase {
 public:
  QSpinBox *widget;
  Setting<int> *setting;
  int multiplier;
  PrefController(QSpinBox *parent, Setting<int> *setting, int multiplier = 1)
      : PrefControllerBase(parent),
        widget(parent),
        setting(setting),
        multiplier(multiplier) {
    connect(widget, &QSpinBox::valueChanged, this, &PrefControllerBase::onChange);
  };
  void loadValue() { widget->setValue(setting->get() / multiplier); }
  void saveValue() { setting->set(widget->value() * multiplier); }
};

template <>
class PrefController<QPlainTextEdit, Setting<QStringList>> : public PrefControllerBase {
 public:
  QPlainTextEdit *widget;
  Setting<QStringList> *setting;
  PrefController(QPlainTextEdit *parent, Setting<QStringList> *setting)
      : PrefControllerBase(parent), widget(parent), setting(setting) {
    connect(widget, &QPlainTextEdit::textChanged, this, &PrefControllerBase::onChange);
  };
  void loadValue() { widget->setPlainText(setting->get().join("\n")); }
  void saveValue() {
    setting->set(widget->toPlainText().split("\n", Qt::SkipEmptyParts));
  }
};

template <>
class PrefController<QLineEdit, Setting<QStringList>> : public PrefControllerBase {
 public:
  QLineEdit *widget;
  Setting<QStringList> *setting;
  PrefController(QLineEdit *parent, Setting<QStringList> *setting)
      : PrefControllerBase(parent), widget(parent), setting(setting) {
    connect(widget, &QLineEdit::textChanged, this, &PrefControllerBase::onChange);
  };
  void loadValue() { widget->setText(setting->get().join(",")); }
  void saveValue() { setting->set(widget->text().split(",", Qt::SkipEmptyParts)); }
};

template <>
class PrefController<QComboBox, Setting<int>> : public PrefControllerBase {
 public:
  QComboBox *widget;
  Setting<int> *setting;
  PrefController(QComboBox *parent, Setting<int> *setting)
      : PrefControllerBase(parent), widget(parent), setting(setting) {
    connect(widget, &QComboBox::currentIndexChanged, this,
            &PrefControllerBase::onChange);
  };
  void loadValue() { widget->setCurrentIndex(widget->findData(setting->get())); }
  void saveValue() { setting->set(widget->currentData().toInt()); }
};

template <>
class PrefController<LanguageSelect, Setting<QString>> : public PrefControllerBase {
 public:
  LanguageSelect *widget;
  Setting<QString> *setting;
  PrefController(LanguageSelect *parent, Setting<QString> *setting)
      : PrefControllerBase(parent), widget(parent), setting(setting) {
    connect(widget, &LanguageSelect::currentIndexChanged, this,
            &PrefControllerBase::onChange);
  };
  void loadValue() { widget->setCurrentIndex(widget->findData(setting->get())); }
  void saveValue() { setting->set(widget->currentData().toString()); }
};

template <>
class PrefController<QComboBox, Setting<QString>> : public PrefControllerBase {
 public:
  QComboBox *widget;
  Setting<QString> *setting;
  PrefController(QComboBox *parent, Setting<QString> *setting)
      : PrefControllerBase(parent), widget(parent), setting(setting) {
    connect(widget, &QComboBox::currentIndexChanged, this,
            &PrefControllerBase::onChange);
    connect(widget, &QComboBox::currentTextChanged, this,
            &PrefControllerBase::onChange);
  };
  void loadValue() { widget->setEditText(setting->get()); }
  void saveValue() { setting->set(widget->currentText()); }
};

class ControllerHolder : public QObject {
  Q_OBJECT

 public:
  ControllerHolder(QObject *parent = 0) : QObject(parent) {};
  PrefControllerBase *add(PrefControllerBase *controller);
  bool isDirty = false;
  void onDirtyChange();
  void load();
  void save();
  void reloadDirty();

 signals:
  void dirtyChanged(bool dirty);

 private:
  QList<PrefControllerBase *> m_controllers = {};
};

namespace Ui {
class PrefWindow;
}

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  PreferenceWindow(SanePreferences *preferences, QWidget *parent = nullptr);
  ~PreferenceWindow();

 private:
  Ui::PrefWindow *ui;
  SanePreferences *preferences;
  QList<QPushButton *> tabButtons;
  QProcess *osaProcess;
  SoundPlayer *soundPlayer;
  ControllerHolder *controllers;
  AutoStart *autoStart;
  bool confirmLeave();
  void setTab(int tabNum);
  void closeEvent(QCloseEvent *event);
  void showEvent(QShowEvent *event);
};
#endif  // SANE_PREFERENCES_WINDOW_H
