// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <qobject.h>

#include <QColor>
#include <QFile>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QtContainerFwd>
#include <functional>
#include <type_traits>

class SettingWithSignal : public QObject {
  Q_OBJECT
 public:
  SettingWithSignal(QObject* parent = 0) : QObject(parent) {}
 signals:
  void changed();
};

template <typename T>
class Setting : public SettingWithSignal {
 public:
  Setting(QSettings* settings, const QString& key, const T& defaultValue,
          QObject* parent = nullptr)
      : SettingWithSignal(parent),
        m_settings(settings),
        m_key(key),
        m_defaultValue(defaultValue),
        m_defaultIsFunction(false) {}
  Setting(QSettings* settings, const QString& key,
          std::function<T()> defaultValueFunction, QObject* parent = nullptr)
      : SettingWithSignal(parent),
        m_settings(settings),
        m_key(key),
        m_defaultValueFunction(defaultValueFunction),
        m_defaultIsFunction(true) {}
  T defaultValue() {
    return m_defaultIsFunction ? m_defaultValueFunction() : m_defaultValue;
  }
  void set(const T& newValue) {
    if (get() == newValue) return;
    m_value = newValue;
    if constexpr (std::is_same_v<T, QColor>) {
      m_settings->setValue(m_key, m_value.name(QColor::HexArgb));
    } else {
      m_settings->setValue(m_key, m_value);
    }
    emit changed();
  }
  const T get() {
    if (m_cached) return m_value;
    if constexpr (std::is_same_v<T, QColor>) {
      m_value = QColor::fromString(
          m_settings->value(m_key, defaultValue().name(QColor::HexArgb)).toString());
    } else {
      m_value = m_settings->value(m_key, defaultValue()).template value<T>();
    }
    m_cached = true;
    return m_value;
  };

 protected:
  QSettings* m_settings;
  QString m_key;
  T m_defaultValue;
  std::function<T()> m_defaultValueFunction;
  bool m_defaultIsFunction;
  T m_value;
  bool m_cached = false;
};

class SanePreferences : public QObject {
  Q_OBJECT
 public:
  SanePreferences(QSettings* settings, QObject* parent = nullptr);
  static SanePreferences* createDefault(QObject* parent = nullptr);
  QSettings* settings;

  Setting<bool>* shownWelcome;

  Setting<int>* smallEvery;
  Setting<int>* smallFor;
  Setting<bool>* bigBreakEnabled;
  Setting<int>* bigAfter;
  Setting<int>* bigFor;

  Setting<int>* postponeMaxMinutePercent;
  Setting<int>* postponeShrinkNextPercent;
  Setting<int>* postponeExtendBreakPercent;

  Setting<int>* flashFor;
  Setting<int>* confirmAfter;
  Setting<int>* maxForceBreakExits;
  Setting<bool>* autoCloseWindowAfterSmallBreak;
  Setting<bool>* autoCloseWindowAfterBigBreak;

  Setting<int>* flashSpeed;
  Setting<QColor>* messageColor;
  Setting<QColor>* countDownColor;
  Setting<QColor>* backgroundColor;
  Setting<QColor>* smallHighlightColor;
  Setting<QColor>* bigHighlightColor;
  Setting<QStringList>* smallMessages;
  Setting<QStringList>* bigMessages;

  Setting<bool>* smallBreakShowProgressBar;
  Setting<bool>* smallBreakShowCountdown;
  Setting<bool>* smallBreakShowClock;
  Setting<bool>* smallBreakShowEndTime;
  Setting<bool>* smallBreakShowButtons;
  Setting<bool>* bigBreakShowProgressBar;
  Setting<bool>* bigBreakShowCountdown;
  Setting<bool>* bigBreakShowClock;
  Setting<bool>* bigBreakShowEndTime;
  Setting<bool>* bigBreakShowButtons;

  Setting<int>* pauseOnIdleFor;
  Setting<int>* resetAfterPause;
  Setting<int>* resetCycleAfterPause;
  Setting<bool>* pauseOnBattery;
  Setting<QStringList>* programsToMonitor;

  Setting<QString>* smallStartBell;
  Setting<QString>* smallEndBell;
  Setting<QString>* bigStartBell;
  Setting<QString>* bigEndBell;

  Setting<int>* autoScreenLock;
  Setting<bool>* quickBreak;
  Setting<QString>* language;
  Setting<bool>* autoStart;
};
