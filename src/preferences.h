// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_H
#define SANE_PREFERENCES_H

#include <QObject>
#include <QSettings>

class SettingWithSignal : public QObject {
  Q_OBJECT
 public:
  SettingWithSignal(QObject *parent = 0) : QObject(parent) {}
 signals:
  void changed();
};

template <typename T>
class Setting : public SettingWithSignal {
 public:
  Setting(const QString &key, const T &defaultValue, QObject *parent = nullptr)
      : SettingWithSignal(parent), key(key), defaultValue(defaultValue) {}
  void set(const T &newValue) {
    QSettings settings;
    if (get() != newValue) {
      value = newValue;
      settings.setValue(key, QVariant::fromValue(value));
      emit changed();
    }
  }
  T get() {
    QSettings settings;
    value = settings.value(key, QVariant::fromValue(defaultValue)).template value<T>();
    return value;
  };

 private:
  QString key;
  T defaultValue;
  T value;
};

class SanePreferences : public QObject {
  Q_OBJECT
 public:
  static Setting<int> *smallEvery;
  static Setting<int> *smallFor;
  static Setting<int> *bigAfter;
  static Setting<int> *bigFor;
  static Setting<int> *flashFor;
  static Setting<int> *pauseOnIdleFor;
  static Setting<bool> *pauseOnBattery;
};

#endif  // SANE_PREFERENCES_H
