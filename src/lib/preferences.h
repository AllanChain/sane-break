// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_H
#define SANE_PREFERENCES_H

#include <QFile>
#include <QObject>
#include <QSettings>
#include <QtContainerFwd>

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
  Setting(QSettings *settings, const QString &key, const T &defaultValue,
          QObject *parent = nullptr)
      : SettingWithSignal(parent),
        m_settings(settings),
        m_key(key),
        m_defaultValue(defaultValue) {}
  void set(const T &newValue) {
    if (get() != newValue) {
      m_value = newValue;
      m_settings->setValue(m_key, QVariant::fromValue(m_value));
      emit changed();
    }
  }
  const T get() {
    if (m_cached) return m_value;
    m_value = m_settings->value(m_key, QVariant::fromValue(m_defaultValue))
                  .template value<T>();
    m_cached = true;
    return m_value;
  };

 private:
  QSettings *m_settings;
  QString m_key;
  T m_defaultValue;
  T m_value;
  bool m_cached = false;
};

class SanePreferences : public QObject {
  Q_OBJECT
 public:
  SanePreferences(QSettings *settings, QObject *parent = nullptr);
  static SanePreferences *createDefault(QObject *parent = nullptr);
  QSettings *settings;

  Setting<bool> *shownWelcome;
  Setting<int> *smallEvery;
  Setting<int> *smallFor;
  Setting<int> *bigAfter;
  Setting<int> *bigFor;
  Setting<int> *flashFor;
  Setting<int> *confirmAfter;
  Setting<int> *flashSpeed;
  Setting<int> *textTransparency;
  Setting<QStringList> *postponeMinutes;
  Setting<int> *autoScreenLock;
  Setting<bool> *quickBreak;
  Setting<QString> *smallStartBell;
  Setting<QString> *smallEndBell;
  Setting<QString> *bigStartBell;
  Setting<QString> *bigEndBell;
  Setting<int> *pauseOnIdleFor;
  Setting<int> *resetAfterPause;
  Setting<int> *resetCycleAfterPause;
  Setting<bool> *pauseOnBattery;
  Setting<QStringList> *programsToMonitor;
  Setting<QString> *language;
  Setting<bool> *autoStart;
};

#endif  // SANE_PREFERENCES_H
