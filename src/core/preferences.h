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
#include <QVariant>
#include <QtContainerFwd>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

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
  // Returns std::nullopt when `value` is valid, otherwise a message explaining why.
  using Validator = std::function<std::optional<QString>(const T&)>;

  Setting(QSettings* settings, const QString& key, const T& defaultValue,
          Validator validator = {}, QObject* parent = nullptr)
      : SettingWithSignal(parent),
        m_settings(settings),
        m_key(key),
        m_defaultValue(defaultValue),
        m_defaultIsFunction(false),
        m_validator(std::move(validator)) {}
  Setting(QSettings* settings, const QString& key,
          std::function<T()> defaultValueFunction, Validator validator = {},
          QObject* parent = nullptr)
      : SettingWithSignal(parent),
        m_settings(settings),
        m_key(key),
        m_defaultValueFunction(defaultValueFunction),
        m_defaultIsFunction(true),
        m_validator(std::move(validator)) {}
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
  // Pure accessor. On first access it loads the value from the settings store and
  // validates it once; later reads return the cached value unchanged.
  const T get() {
    if (!m_cached) {
      if constexpr (std::is_same_v<T, QColor>) {
        m_value = QColor::fromString(
            m_settings->value(m_key, defaultValue().name(QColor::HexArgb)).toString());
      } else {
        m_value = m_settings->value(m_key, defaultValue()).template value<T>();
      }
      validate();
      m_cached = true;
    }
    return m_value;
  }

 protected:
  // Validates m_value against m_validator. On failure it emits a clear warning and
  // replaces m_value with the configured default. Invoked only on first load (get),
  // the single point where untrusted config-file data enters the setting.
  void validate() {
    if (!m_validator) return;
    if (auto reason = m_validator(m_value)) {
      qWarning().noquote() << QStringLiteral(
                                  "Invalid value for setting \"%1\": %2 rejected (%3); "
                                  "using default %4")
                                  .arg(m_key)
                                  .arg(QVariant(m_value).toString())
                                  .arg(*reason)
                                  .arg(QVariant(defaultValue()).toString());
      m_value = defaultValue();
    }
  }

  QSettings* m_settings;
  QString m_key;
  T m_defaultValue;
  std::function<T()> m_defaultValueFunction;
  bool m_defaultIsFunction;
  Validator m_validator;
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

  Setting<int>* focusSmallEvery;
  Setting<int>* focusSmallFor;
  Setting<bool>* focusBigBreakEnabled;
  Setting<int>* focusBigAfter;
  Setting<int>* focusBigFor;

  Setting<int>* postponeMaxMinutePercent;
  Setting<int>* postponeShrinkNextPercent;
  Setting<int>* postponeExtendBreakPercent;

  Setting<int>* minReasonLength;

  Setting<int>* flashFor;
  Setting<int>* confirmAfter;
  Setting<int>* flashTrayFor;
  Setting<int>* headsUpFor;
  Setting<int>* maxForceBreakExits;
  Setting<bool>* autoCloseWindowAfterSmallBreak;
  Setting<bool>* autoCloseWindowAfterBigBreak;

  Setting<int>* flashSpeed;
  Setting<QColor>* messageColor;
  Setting<QColor>* countDownColor;
  Setting<QColor>* smallBackgroundColor;
  Setting<QColor>* bigBackgroundColor;
  Setting<QColor>* smallFlashColor;
  Setting<QColor>* bigFlashColor;
  Setting<QString>* smallBackgroundImage;
  Setting<QString>* bigBackgroundImage;
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
  // When true (default), an app keeping the screen awake (video player,
  // presentation) counts as activity and the idle pause won't trigger.
  // Mapped to IdleMode in app.cpp.
  Setting<bool>* treatInhibitorAsActivity;
  Setting<QStringList>* programsToMonitor;
  Setting<bool>* pauseOnUnknownMonitor;
  Setting<QStringList>* knownMonitors;

  Setting<QString>* smallStartBell;
  Setting<QString>* smallEndBell;
  Setting<QString>* bigStartBell;
  Setting<QString>* bigEndBell;

  Setting<int>* autoScreenLock;
  Setting<bool>* quickBreak;
  Setting<QString>* language;
  Setting<bool>* autoStart;
};
