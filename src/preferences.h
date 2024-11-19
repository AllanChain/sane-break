// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_H
#define SANE_PREFERENCES_H

#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QSettings>

class SettingWithSignal : public QObject {
  Q_OBJECT
 public:
  SettingWithSignal(QObject *parent = 0) : QObject(parent) {}
  QSettings getSettings() {
    // We prefer settings file next to the app executable to make app more portable
    QFile portableSettings(QCoreApplication::applicationDirPath() + "/SaneBreak.ini");
    if (!portableSettings.exists()) return QSettings();
    return QSettings(portableSettings.fileName(), QSettings::IniFormat);
  };
 signals:
  void changed();
};

template <typename T>
class Setting : public SettingWithSignal {
 public:
  Setting(const QString &key, const T &defaultValue, QObject *parent = nullptr)
      : SettingWithSignal(parent), key(key), defaultValue(defaultValue) {}
  void set(const T &newValue) {
    QSettings settings = getSettings();
    if (get() != newValue) {
      value = newValue;
      settings.setValue(key, QVariant::fromValue(value));
      emit changed();
    }
  }
  T get() {
    if (cached) return value;
    QSettings settings = getSettings();
    value = settings.value(key, QVariant::fromValue(defaultValue)).template value<T>();
    cached = true;
    return value;
  };

 private:
  QString key;
  T defaultValue;
  T value;
  bool cached = false;
};

class SanePreferences : public QObject {
  Q_OBJECT
 public:
  static Setting<int> *smallEvery;
  static Setting<int> *smallFor;
  static Setting<int> *bigAfter;
  static Setting<int> *bigFor;
  static Setting<int> *flashFor;
  static Setting<QString> *bellStart;
  static Setting<QString> *bellEnd;
  static Setting<int> *pauseOnIdleFor;
  static Setting<int> *resetAfterPause;
  static Setting<int> *resetCycleAfterPause;
  static Setting<bool> *pauseOnBattery;
};

#endif  // SANE_PREFERENCES_H
