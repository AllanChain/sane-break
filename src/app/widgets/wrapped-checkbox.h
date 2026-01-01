// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

// This file contains code derived from a StackOverflow post, licensed under the
// Creative Commons Attribution-ShareAlike 4.0 International (CC-BY-SA-4.0).
//
// Original Author: emkey08
// Source: https://stackoverflow.com/a/75386485/8810271
//
// You are required to comply with the terms of the CC-BY-SA-4.0 license when using
// this portion of the code. For more details, see:
// <https://creativecommons.org/licenses/by-sa/4.0/>.

#pragma once

#include <QCheckBox>
#include <QSize>
#include <QString>
#include <QWidget>

class WrappedCheckBox : public QCheckBox {
  Q_OBJECT
 public:
  WrappedCheckBox(QWidget* parent = nullptr) : WrappedCheckBox(QString(), parent) {}
  WrappedCheckBox(const QString& text, QWidget* parent = nullptr);
  QSize minimumSizeHint() const override {
    return QSize(QCheckBox().minimumSizeHint().width(), sizeHint().height());
  }

 private:
  void wrapLines(int width);

 protected:
  void resizeEvent(QResizeEvent* event) override;
};
