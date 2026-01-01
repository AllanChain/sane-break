// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QLineEdit>
#include <QPixmap>
#include <QWidget>

class ColorEdit : public QLineEdit {
  Q_OBJECT
 public:
  ColorEdit(QWidget* parent = nullptr) : QLineEdit(parent) {
    setInputMask("\\#HHHHHHHH");
    connect(this, &QLineEdit::textChanged, this, [this]() {
      QPixmap icon(10, 10);
      icon.fill(this->text());
      if (actions().empty())
        addAction(icon, QLineEdit::TrailingPosition);
      else
        actions()[0]->setIcon(icon);
    });
  };
};
