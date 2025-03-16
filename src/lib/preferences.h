// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_H
#define SANE_PREFERENCES_H

#include <QFile>
#include <QObject>
#include <QSettings>
#include <QtContainerFwd>

QSettings getSettings();

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
    QSettings settings = getSettings();
    if (get() != newValue) {
      value = newValue;
      settings.setValue(key, QVariant::fromValue(value));
      emit changed();
    }
  }
  const T get() {
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
  static Setting<bool> *shownWelcome;
  static Setting<int> *smallEvery;
  static Setting<int> *smallFor;
  static Setting<int> *bigAfter;
  static Setting<int> *bigFor;
  static Setting<int> *flashFor;
  static Setting<int> *confirmAfter;
  static Setting<QStringList> *postponeMinutes;
  static Setting<int> *autoScreenLock;
  static Setting<bool> *quickBreak;
  static Setting<QString> *smallStartBell;
  static Setting<QString> *smallEndBell;
  static Setting<QString> *bigStartBell;
  static Setting<QString> *bigEndBell;
  static Setting<int> *pauseOnIdleFor;
  static Setting<int> *resetAfterPause;
  static Setting<int> *resetCycleAfterPause;
  static Setting<bool> *pauseOnBattery;
  static Setting<QStringList> *programsToMonitor;
  static Setting<QString> *language;
  static Setting<bool> *autoStart;
};

#endif  // SANE_PREFERENCES_H
