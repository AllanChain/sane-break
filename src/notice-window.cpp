// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notice-window.h"

#include <QFile>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>

NoticeWindow::NoticeWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon.png"));
  setAttribute(Qt::WA_DeleteOnClose);
  resize(620, 600);

  QScrollArea *scrollArea = new QScrollArea(this);
  setCentralWidget(scrollArea);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QFile noticeFile(":/NOTICE.md");
  noticeFile.open(QIODevice::ReadOnly | QIODevice::Text);
  QLabel *textLabel = new QLabel(noticeFile.readAll());
  textLabel->setContentsMargins(20, 20, 20, 0);
  textLabel->setTextFormat(Qt::MarkdownText);
  textLabel->setWordWrap(true);
  textLabel->setOpenExternalLinks(true);
  scrollArea->setWidget(textLabel);
}
