// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2024-2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QLabel>
#include <QString>
#include <QWidget>

class DynamicLabel : public QLabel {
  Q_OBJECT
 public:
  using QLabel::QLabel;
  ~DynamicLabel() override = default;

  void setText(const QString& text) {
    m_baseText = text;
    updateText();
  };
  const QVariantList args() { return m_args; };
  void setArgs(const QVariantList& args) {
    m_args = args;
    updateText();
  };

 public slots:
  void updateText() {
    QString formattedText = m_baseText;
    for (const auto& arg : args()) {
      formattedText = formattedText.arg(arg.toString());
    }
    QLabel::setText(formattedText);
  };

 private:
  QString m_baseText;
  QVariantList m_args;
};
