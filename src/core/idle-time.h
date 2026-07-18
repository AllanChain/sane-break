// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <qglobal.h>

#include <QObject>
#include <QTimer>
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#include <QtCompilerDetection>
#endif

#if defined(QT_SHARED) || !defined(QT_STATIC)
#if defined(BUILD_COREIDLE_LIB)
#define COREIDLE_EXPORT Q_DECL_EXPORT
#else
#define COREIDLE_EXPORT Q_DECL_IMPORT
#endif
#else
#define COREIDLE_EXPORT
#endif

// How idle is measured. The enum describes the *mechanism*; the user-facing
// policy is a single bool preference mapped to this enum in app.cpp.
//   InputOnly       - input-only idle (Wayland v2; polling backends use the raw
//                     reader). Pause still applies during a video (old behavior).
//   InhibitorAware  - inhibitor-aware idle (Wayland v1; polling backends wrap the
//                     raw reader so idle is held at 0 while an inhibitor is
//                     active). Don't pause during video (default).
enum class IdleMode { InputOnly, InhibitorAware };

class COREIDLE_EXPORT SystemIdleTime : public QObject {
  Q_OBJECT
 public:
  SystemIdleTime(QObject* parent = nullptr) : QObject(parent) {}
  virtual void startWatching() = 0;
  virtual void stopWatching() = 0;
  int watchAccuracy() { return m_watchAccuracy; }
  int minIdleTime() { return m_minIdleTime; }
  virtual void setWatchAccuracy(int accuracy) = 0;
  virtual void setMinIdleTime(int idleTime) = 0;
  // Select how idle is measured. Default no-op just records the mode; backends
  // that support inhibitor-aware idle (Wayland v1/v2 switch, or polling backends
  // with an inhibitor monitor wired in) override this.
  virtual void setIdleMode(IdleMode mode) { m_idleMode = mode; }
  IdleMode idleMode() const { return m_idleMode; }
  bool isIdle() { return m_isIdle; }
 signals:
  void idleStart();
  void idleEnd();

 protected:
  bool m_isIdle = false;
  int m_watchAccuracy = 500;  // How often we watch idle time (ms)
  int m_minIdleTime = 2000;   // How long will we consider idle (ms)
  IdleMode m_idleMode = IdleMode::InhibitorAware;  // matches pref default (on)
};
