// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QColorDialog>
#include <QEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QString>
#include <QWidget>

class ColorEdit : public QLineEdit {
  Q_OBJECT
 public:
  ColorEdit(QWidget* parent = nullptr) : QLineEdit(parent) {
    setInputMask("\\#HHHHHHHH");
    m_colorAction = addAction(QIcon(), QLineEdit::TrailingPosition);
    connect(this, &QLineEdit::textChanged, this, &ColorEdit::updateIcon);
    connect(m_colorAction, &QAction::triggered, this, &ColorEdit::pickColor);
  };

 protected:
  void changeEvent(QEvent* event) override {
    QLineEdit::changeEvent(event);
    if (event->type() == QEvent::PaletteChange) updateIcon();
  }

 private:
  QAction* m_colorAction;

  void updateIcon() {
    QPixmap icon(10, 10);
    QPainter painter(&icon);
    painter.setPen(palette().color(QPalette::Text));
    painter.setBrush(QColor(text()));
    painter.drawRect(icon.rect().adjusted(0, 0, -1, -1));
    m_colorAction->setIcon(icon);
  }

  void pickColor() {
    QColor color = QColorDialog::getColor(QColor(text()), this, QString(),
                                          QColorDialog::ShowAlphaChannel);
    if (color.isValid()) setText(color.name(QColor::HexArgb));
  }
};
