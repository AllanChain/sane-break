// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

class AbstractTimer : public QObject {
  Q_OBJECT
 public:
  AbstractTimer(QObject* parent = nullptr) : QObject(parent) {};
  ~AbstractTimer() = default;
  virtual void start() { m_active = true; };
  virtual void start(int msec) { m_active = m_interval = msec; };
  virtual void stop() { m_active = false; };
  virtual bool isActive() { return m_active; }
  virtual void setSingleShot(bool singleShot) { m_singleShot = singleShot; };
  virtual bool isSingleShot() { return m_singleShot; };
  virtual void setInterval(int msec) { m_interval = msec; };
  virtual int interval() { return m_interval; }
 signals:
  void timeout();

 private:
  bool m_singleShot = false;
  bool m_interval = 0;
  bool m_active = false;
};
