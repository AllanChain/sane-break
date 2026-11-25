// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QAbstractItemModel>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMap>
#include <QObject>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <functional>

#include "app/widgets/language-select.h"
#include "core/preferences.h"

class PrefControllerBase : public QObject {
  Q_OBJECT
 public:
  explicit PrefControllerBase(QObject* parent = nullptr) : QObject(parent) {}
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
  void explicitSync();
  void dirtyChanged();
};

// PrefControllerBase defines common behaviors without template.
// PrefControllerTemplate defines behaviors with the template.
template <typename W, typename T>
class PrefControllerTemplate : public PrefControllerBase {
 public:
  W* widget;
  Setting<T>* setting;
  PrefControllerTemplate(W* widget, Setting<T>* setting, QObject* parent = nullptr)
      : PrefControllerBase(parent), widget(widget), setting(setting) {}
  virtual void setValue(T) = 0;
  void loadValue() override { setValue(setting->get()); }
  void setToDefault() override { setValue(setting->defaultValue()); }
};

template <typename W, typename T>
class PrefController : public PrefControllerTemplate<W, T> {};

template <>
class PrefController<QCheckBox, bool> : public PrefControllerTemplate<QCheckBox, bool> {
 public:
  PrefController(QCheckBox* widget, Setting<bool>* setting, QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    connect(widget, &QCheckBox::checkStateChanged, this, &PrefControllerBase::onChange);
#else
    connect(widget, &QCheckBox::stateChanged, this, &PrefControllerBase::onChange);
#endif
  }
  void setValue(bool value) override { widget->setChecked(value); }
  void saveValue() override { setting->set(widget->isChecked()); }
};

template <>
class PrefController<QSpinBox, int> : public PrefControllerTemplate<QSpinBox, int> {
 public:
  int multiplier;
  PrefController(QSpinBox* widget, Setting<int>* setting, int multiplier = 1,
                 QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent), multiplier(multiplier) {
    connect(widget, &QSpinBox::valueChanged, this, &PrefControllerBase::onChange);
  }
  void setValue(int value) override { widget->setValue(value / multiplier); }
  void saveValue() override { setting->set(widget->value() * multiplier); }
};

template <>
class PrefController<QPlainTextEdit, QStringList>
    : public PrefControllerTemplate<QPlainTextEdit, QStringList> {
 public:
  PrefController(QPlainTextEdit* widget, Setting<QStringList>* setting,
                 QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
    connect(widget, &QPlainTextEdit::textChanged, this, &PrefControllerBase::onChange);
  }
  void setValue(QStringList value) override { widget->setPlainText(value.join("\n")); }
  void saveValue() override {
    setting->set(widget->toPlainText().split("\n", Qt::SkipEmptyParts));
  }
};

template <>
class PrefController<QListWidget, QStringList>
    : public PrefControllerTemplate<QListWidget, QStringList> {
 public:
  PrefController(QListWidget* widget, Setting<QStringList>* setting,
                 QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
    auto* model = widget->model();
    connect(model, &QAbstractItemModel::rowsInserted, this,
            &PrefControllerBase::onChange);
    connect(model, &QAbstractItemModel::rowsRemoved, this,
            &PrefControllerBase::onChange);
    connect(model, &QAbstractItemModel::dataChanged, this,
            &PrefControllerBase::onChange);
  }
  void setValue(QStringList value) override {
    widget->clear();
    widget->addItems(value);
  }
  void saveValue() override {
    QStringList items;
    for (int i = 0; i < widget->count(); ++i) {
      items.append(widget->item(i)->text());
    }
    setting->set(items);
  }
};

template <>
class PrefController<QComboBox, int> : public PrefControllerTemplate<QComboBox, int> {
 public:
  PrefController(QComboBox* widget, Setting<int>* setting, QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
    connect(widget, &QComboBox::currentIndexChanged, this,
            &PrefControllerBase::onChange);
  }
  void setValue(int value) override {
    widget->setCurrentIndex(widget->findData(value));
  }
  void saveValue() override { setting->set(widget->currentData().toInt()); }
};

template <>
class PrefController<LanguageSelect, QString>
    : public PrefControllerTemplate<LanguageSelect, QString> {
 public:
  PrefController(LanguageSelect* widget, Setting<QString>* setting,
                 QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
    connect(widget, &LanguageSelect::currentIndexChanged, this,
            &PrefControllerBase::onChange);
  }
  void setValue(QString value) override {
    widget->setCurrentIndex(widget->findData(value));
  }
  void saveValue() override { setting->set(widget->currentData().toString()); }
};

template <>
class PrefController<QComboBox, QString>
    : public PrefControllerTemplate<QComboBox, QString> {
 public:
  PrefController(QComboBox* widget, Setting<QString>* setting,
                 QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
    connect(widget, &QComboBox::currentIndexChanged, this,
            &PrefControllerBase::onChange);
    connect(widget, &QComboBox::currentTextChanged, this,
            &PrefControllerBase::onChange);
  }
  void setValue(QString value) override { widget->setEditText(value); }
  void saveValue() override { setting->set(widget->currentText()); }
};

template <>
class PrefController<QLineEdit, QString>
    : public PrefControllerTemplate<QLineEdit, QString> {
 public:
  PrefController(QLineEdit* widget, Setting<QString>* setting,
                 QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
    connect(widget, &QLineEdit::textChanged, this, &PrefControllerBase::onChange);
  }
  void setValue(QString value) override { widget->setText(value); }
  void saveValue() override { setting->set(widget->text()); }
};

template <>
class PrefController<QLineEdit, QColor>
    : public PrefControllerTemplate<QLineEdit, QColor> {
 public:
  PrefController(QLineEdit* widget, Setting<QColor>* setting, QObject* parent = nullptr)
      : PrefControllerTemplate(widget, setting, parent) {
    connect(widget, &QLineEdit::textChanged, this, &PrefControllerBase::onChange);
  }
  void setValue(QColor value) override { widget->setText(value.name(QColor::HexArgb)); }
  void saveValue() override { setting->set(widget->text()); }
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
  explicit ControllerHolder(QObject* parent = nullptr) : QObject(parent) {}
  PrefControllerBase* add(PrefGroup group, PrefControllerBase* controller);
  bool isDirty = false;
  void onDirtyChange();
  void load();
  void save();
  void reloadDirty();
  void setGroupToDefault(PrefGroup group);
  void forEach(std::function<void(PrefControllerBase*)> func);

 signals:
  void dirtyChanged(bool dirty);

 private:
  QMap<PrefGroup, QList<PrefControllerBase*>> m_controllers = {};
};
