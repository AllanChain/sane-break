// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QScrollArea>

class TextWindow : public QScrollArea {
  Q_OBJECT
 public:
  TextWindow(const QString &text, QWidget *parent = nullptr);
  ~TextWindow() = default;
};
