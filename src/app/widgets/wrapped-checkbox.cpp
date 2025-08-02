// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2025 Sane Break developers
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

#include "wrapped-checkbox.h"

#include <QCheckBox>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QString>
#include <QStyle>
#include <QWidget>
#include <Qt>

WrappedCheckBox::WrappedCheckBox(const QString &text, QWidget *parent)
    : QCheckBox(text, parent) {
  QSizePolicy policy = sizePolicy();
  policy.setHorizontalPolicy(QSizePolicy::Preferred);
  setSizePolicy(policy);
  updateGeometry();
}

void WrappedCheckBox::wrapLines(int width) {
  QString word, line, result;
  for (QChar c : text().replace('\n', ' ') + ' ') {
    word += c;
    if (c.isSpace()) {
      if (!line.isEmpty() &&
          fontMetrics().horizontalAdvance(line + word.trimmed()) > width) {
        result += line.trimmed() + '\n';
        line = word;
      } else {
        line += word;
      }
      word.clear();
    }
  }
  result += line.trimmed();
  setText(result.trimmed());
}

void WrappedCheckBox::resizeEvent(QResizeEvent *event) {
  int controlElementWidth =
      sizeHint().width() -
      style()
          ->itemTextRect(fontMetrics(), QRect(), Qt::TextShowMnemonic, false, text())
          .width();
  wrapLines(event->size().width() - controlElementWidth);
  QCheckBox::resizeEvent(event);
}
