// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QCheckBox>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QMap>
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
#include <functional>

#include "app/break-windows.h"
#include "app/text-window.h"
#include "core/preferences.h"
#include "lib/auto-start.h"
#include "sound-player.h"
#include "widgets/language-select.h"

class PrefControllerBase : public QObject {
  Q_OBJECT
 public:
  PrefControllerBase(QObject* parent = 0) : QObject(parent) {}
  bool isDirty = false;
  bool changeMeansDirty = true;
  void saveIfDirty();
  void load();
  virtual void saveValue() = 0;
  virtual void loadValue() = 0;
  virtual void setToDefault() = 0;
  void onChange();
 signals:
  void loaded();
  void changed();
  // This is like changed for underlying widgets, but also emits when values are loaded
  // from settings (which does not necessarily change the value of the widget). This is
  // useful when we want to sync the labels with the value of the setting.
  void explictSync();
  void dirtyChanged();
};

// PrefControllerBase defines common behaviors without template.
// PrefControllerTemplate defined behaviors with the template.
template <typename W, typename T>
class PrefControllerTemplate : public PrefControllerBase {
 public:
  W* widget;
  Setting<T>* setting;
  PrefControllerTemplate(W* parent, Setting<T>* setting)
      : widget(parent), setting(setting) {}
  virtual void setValue(T) = 0;
  void loadValue() override { setValue(setting->get()); }
  void setToDefault() override { setValue(setting->defaultValue()); }
};

template <typename W, typename T>
class PrefController : public PrefControllerTemplate<W, T> {};

template <>
class PrefController<QCheckBox, bool> : public PrefControllerTemplate<QCheckBox, bool> {
 public:
  PrefController(QCheckBox* parent, Setting<bool>* setting)
      : PrefControllerTemplate(parent, setting) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    connect(widget, &QCheckBox::checkStateChanged, this, &PrefControllerBase::onChange);
#else
    connect(widget, &QCheckBox::stateChanged, this, &PrefControllerBase::onChange);
#endif
  };
  void setValue(bool value) { widget->setChecked(value); }
  void saveValue() { setting->set(widget->isChecked()); }
};

template <>
class PrefController<QSlider, int> : public PrefControllerTemplate<QSlider, int> {
 public:
  int multiplier;
  PrefController(QSlider* parent, Setting<int>* setting, int multiplier = 1)
      : PrefControllerTemplate(parent, setting), multiplier(multiplier) {
    connect(widget, &QSlider::valueChanged, this, &PrefControllerBase::onChange);
  };
  void setValue(int value) { widget->setValue(value / multiplier); }
  void saveValue() { setting->set(widget->value() * multiplier); }
};

template <>
class PrefController<QSpinBox, int> : public PrefControllerTemplate<QSpinBox, int> {
 public:
  int multiplier;
  PrefController(QSpinBox* parent, Setting<int>* setting, int multiplier = 1)
      : PrefControllerTemplate(parent, setting), multiplier(multiplier) {
    connect(widget, &QSpinBox::valueChanged, this, &PrefControllerBase::onChange);
  };
  void setValue(int value) { widget->setValue(value / multiplier); }
  void saveValue() { setting->set(widget->value() * multiplier); }
};

template <>
class PrefController<QPlainTextEdit, QStringList>
    : public PrefControllerTemplate<QPlainTextEdit, QStringList> {
 public:
  PrefController(QPlainTextEdit* parent, Setting<QStringList>* setting)
      : PrefControllerTemplate(parent, setting) {
    connect(widget, &QPlainTextEdit::textChanged, this, &PrefControllerBase::onChange);
  };
  void setValue(QStringList value) { widget->setPlainText(value.join("\n")); }
  void saveValue() {
    setting->set(widget->toPlainText().split("\n", Qt::SkipEmptyParts));
  }
};

template <>
class PrefController<QLineEdit, QStringList>
    : public PrefControllerTemplate<QLineEdit, QStringList> {
 public:
  PrefController(QLineEdit* parent, Setting<QStringList>* setting)
      : PrefControllerTemplate(parent, setting) {
    connect(widget, &QLineEdit::textChanged, this, &PrefControllerBase::onChange);
  };
  void setValue(QStringList value) { widget->setText(value.join(",")); }
  void saveValue() { setting->set(widget->text().split(",", Qt::SkipEmptyParts)); }
};

template <>
class PrefController<QComboBox, int> : public PrefControllerTemplate<QComboBox, int> {
 public:
  PrefController(QComboBox* parent, Setting<int>* setting)
      : PrefControllerTemplate(parent, setting) {
    connect(widget, &QComboBox::currentIndexChanged, this,
            &PrefControllerBase::onChange);
  };
  void setValue(int value) { widget->setCurrentIndex(widget->findData(value)); }
  void saveValue() { setting->set(widget->currentData().toInt()); }
};

template <>
class PrefController<LanguageSelect, QString>
    : public PrefControllerTemplate<LanguageSelect, QString> {
 public:
  PrefController(LanguageSelect* parent, Setting<QString>* setting)
      : PrefControllerTemplate(parent, setting) {
    connect(widget, &LanguageSelect::currentIndexChanged, this,
            &PrefControllerBase::onChange);
  };
  void setValue(QString value) { widget->setCurrentIndex(widget->findData(value)); }
  void saveValue() { setting->set(widget->currentData().toString()); }
};

template <>
class PrefController<QComboBox, QString>
    : public PrefControllerTemplate<QComboBox, QString> {
 public:
  PrefController(QComboBox* parent, Setting<QString>* setting)
      : PrefControllerTemplate(parent, setting) {
    connect(widget, &QComboBox::currentIndexChanged, this,
            &PrefControllerBase::onChange);
    connect(widget, &QComboBox::currentTextChanged, this,
            &PrefControllerBase::onChange);
  };
  void setValue(QString value) { widget->setEditText(value); }
  void saveValue() { setting->set(widget->currentText()); }
};

template <>
class PrefController<QLineEdit, QColor>
    : public PrefControllerTemplate<QLineEdit, QColor> {
 public:
  PrefController(QLineEdit* parent, Setting<QColor>* setting)
      : PrefControllerTemplate(parent, setting) {
    connect(widget, &QLineEdit::textChanged, this, &PrefControllerBase::onChange);
  };
  void setValue(QColor value) { widget->setText(value.name(QColor::HexArgb)); }
  void saveValue() { setting->set(widget->text()); }
};

enum class PrefGroup {
  Schedule = 0,
  Reminder = 1,
  Interface = 2,
  Pause = 3,
  Sound = 4,
  General = 5,
};

class ControllerHolder : public QObject {
  Q_OBJECT

 public:
  ControllerHolder(QObject* parent = 0) : QObject(parent) {};
  PrefControllerBase* add(PrefGroup, PrefControllerBase*);
  bool isDirty = false;
  void onDirtyChange();
  void load();
  void save();
  void reloadDirty();
  void setGroupToDefault(PrefGroup);
  void forEach(std::function<void(PrefControllerBase*)>);

 signals:
  void dirtyChanged(bool dirty);

 private:
  QMap<PrefGroup, QList<PrefControllerBase*>> m_controllers = {};
};

namespace Ui {
class PrefWindow;
}

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  PreferenceWindow(SanePreferences* preferences, QWidget* parent = nullptr);
  ~PreferenceWindow();

 private:
  Ui::PrefWindow* ui;
  SanePreferences* preferences;
  QList<QPushButton*> tabButtons;
  SoundPlayer* soundPlayer;
  ControllerHolder* controllers;
  AutoStart* autoStart;
  TextWindow* openingTextWindow = nullptr;
  BreakWindows* breakWindows;

  bool confirmLeave();
  void setTab(int tabNum, int totalTabCount);
  void forceThemeUpdate();
  void openNotice();
  void openSourceCode();
  void openWeblate();
  void openBreakWindowPreview();
  void closeEvent(QCloseEvent* event);
  void showEvent(QShowEvent* event);
  void changeEvent(QEvent* event);
};
