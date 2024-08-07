// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_NOTICE_WINDOW_H
#define SANE_NOTICE_WINDOW_H

#include <QMainWindow>

class NoticeWindow : public QMainWindow {
  Q_OBJECT

 public:
  NoticeWindow(QWidget *parent = nullptr);
};
#endif  // SANE_NOTICE_WINDOW_H
