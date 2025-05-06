// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include "text-window.h"

#include <QLabel>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>

TextWindow::TextWindow(const QString& text, QWidget* parent) : QScrollArea(parent) {
  setFixedSize(500, 500);
  setWidgetResizable(true);

  QWidget* widget = new QWidget();
  setWidget(widget);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(12, 12, 12, 12);
  widget->setLayout(layout);

  QLabel* label = new QLabel(text);
  label->setTextInteractionFlags(Qt::TextSelectableByMouse);
  label->setOpenExternalLinks(true);
  label->setTextFormat(Qt::MarkdownText);
  label->setWordWrap(true);
  layout->addWidget(label);
}
