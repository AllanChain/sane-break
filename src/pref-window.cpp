// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pref-window.h"

#include <QDesktopServices>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QVBoxLayout>

#include "config.h"
#include "default-pref.h"
#include "notice-window.h"

PreferenceWindow::PreferenceWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon.png"));
  resize(400, 500);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  centralWidget->setContentsMargins(10, 10, 10, 10);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);
  layout->setAlignment(Qt::AlignCenter);
  layout->setSpacing(20);

  QHBoxLayout *titleLayout = new QHBoxLayout();
  titleLayout->setAlignment(Qt::AlignLeft);

  QLabel *imageLabel = new QLabel();
  QPixmap pixmap = QPixmap(":/images/icon-256.png");
  pixmap.setDevicePixelRatio(2.0);
  imageLabel->setPixmap(pixmap);
  imageLabel->setAlignment(Qt::AlignCenter);
  imageLabel->setMargin(20);
  titleLayout->addWidget(imageLabel);

  QVBoxLayout *titleTextLayout = new QVBoxLayout();
  titleTextLayout->setAlignment(Qt::AlignCenter);

  QLabel *title = new QLabel("Sane Break");
  title->setProperty("title", "h1");
  titleTextLayout->addWidget(title);

  QLabel *copyrightLabel = new QLabel(
      QString("<p>A polite break reminder to keep you sane.</p>"
              "<p>Copyright (C) 2024 Allan Chain, "
              "<a href=https://www.gnu.org/licenses/gpl-3.0.en.html>GPL 3.0</a>"
              "<br>Version v%1. Source available at "
              "<a href=https://github.com/AllanChain/sane-break>GitHub</a></p>"
              "<p><a href=notice-window>Third party libraries notices</a></p>")
          .arg(PROJECT_VERSION));
  copyrightLabel->setTextFormat(Qt::RichText);
  connect(copyrightLabel, &QLabel::linkActivated, this, [=](QString url) {
    if (url == QString("notice-window"))
      (new NoticeWindow(this))->show();
    else
      QDesktopServices::openUrl(url);
  });
  titleTextLayout->addWidget(copyrightLabel);

  titleLayout->addLayout(titleTextLayout);
  layout->addLayout(titleLayout);

  QLabel *titleSmall = new QLabel("Small Break");
  titleSmall->setProperty("title", "h2");
  layout->addWidget(titleSmall);

  QGridLayout *smallBreakForm = new QGridLayout();
  layout->addLayout(smallBreakForm);

  smallBreakForm->addWidget(new QLabel("Every"), 0, 0);
  smallBreakForm->addWidget(new QLabel("For"), 1, 0);

  smallBreakEverySlider = new QSlider(Qt::Horizontal);
  smallBreakEverySlider->setMaximum(60);
  smallBreakEverySlider->setTickPosition(QSlider::TicksBelow);
  smallBreakForm->addWidget(smallBreakEverySlider, 0, 1);

  QLabel *smallBreakEveryLabel = new QLabel();
  smallBreakForm->addWidget(smallBreakEveryLabel, 0, 2);
  connect(smallBreakEverySlider, &QSlider::valueChanged, this,
          [smallBreakEveryLabel](int value) {
            smallBreakEveryLabel->setText(QString("%1 min").arg(value));
          });

  smallBreakForSlider = new QSlider(Qt::Horizontal);
  smallBreakForSlider->setMaximum(60);
  smallBreakForSlider->setTickPosition(QSlider::TicksBelow);
  smallBreakForm->addWidget(smallBreakForSlider, 1, 1);

  QLabel *smallBreakForLabel = new QLabel();
  smallBreakForm->addWidget(smallBreakForLabel, 1, 2);
  connect(smallBreakForSlider, &QSlider::valueChanged, this,
          [smallBreakForLabel](int value) {
            smallBreakForLabel->setText(QString("%1 sec").arg(value));
          });

  QLabel *titleBig = new QLabel("Big Break");
  titleBig->setProperty("title", "h2");
  layout->addWidget(titleBig);

  QGridLayout *bigBreakForm = new QGridLayout();
  layout->addLayout(bigBreakForm);

  bigBreakForm->addWidget(new QLabel("After"), 0, 0);
  bigBreakForm->addWidget(new QLabel("For"), 1, 0);

  bigBreakAfterSlider = new QSlider(Qt::Horizontal);
  bigBreakAfterSlider->setMaximum(20);
  bigBreakAfterSlider->setTickPosition(QSlider::TicksBelow);
  bigBreakForm->addWidget(bigBreakAfterSlider, 0, 1);

  QLabel *bigBreakAfterLabel = new QLabel();
  bigBreakForm->addWidget(bigBreakAfterLabel, 0, 2);
  connect(bigBreakAfterSlider, &QSlider::valueChanged, this,
          [bigBreakAfterLabel](int value) {
            bigBreakAfterLabel->setText(QString("%1 small breaks").arg(value));
          });

  bigBreakForSlider = new QSlider(Qt::Horizontal);
  bigBreakForSlider->setMaximum(300);
  bigBreakForSlider->setSingleStep(10);
  bigBreakForSlider->setTickInterval(60);
  bigBreakForSlider->setPageStep(60);
  bigBreakForSlider->setTickPosition(QSlider::TicksBelow);
  bigBreakForm->addWidget(bigBreakForSlider, 1, 1);

  QLabel *bigBreakForLabel = new QLabel();
  bigBreakForm->addWidget(bigBreakForLabel, 1, 2);
  connect(bigBreakForSlider, &QSlider::valueChanged, this,
          [bigBreakForLabel](int value) {
            bigBreakForLabel->setText(QString("%1 sec").arg(value));
          });

  QHBoxLayout *bottomButtonLayout = new QHBoxLayout();
  QPushButton *resetButton = new QPushButton("Reset");
  connect(resetButton, &QPushButton::pressed, this,
          &PreferenceWindow::loadSettings);
  bottomButtonLayout->addWidget(resetButton);
  bottomButtonLayout->addStretch();
  QPushButton *saveButton = new QPushButton("Save");
  connect(saveButton, &QPushButton::pressed, this,
          &PreferenceWindow::saveSettings);
  bottomButtonLayout->addWidget(saveButton);
  layout->addLayout(bottomButtonLayout);
}

void PreferenceWindow::loadSettings() {
  QSettings settings;
  smallBreakEverySlider->setValue(
      settings.value("break/small-every", SANE_BREAK_SMALL_EVERY).toInt() / 60);
  smallBreakForSlider->setValue(
      settings.value("break/small-for", SANE_BREAK_SMALL_FOR).toInt());
  bigBreakAfterSlider->setValue(
      settings.value("break/big-after", SANE_BREAK_BIG_AFTER).toInt());
  bigBreakForSlider->setValue(
      settings.value("break/big-for", SANE_BREAK_BIG_FOR).toInt());
}

void PreferenceWindow::saveSettings() {
  QSettings settings;
  settings.setValue("break/small-every", smallBreakEverySlider->value() * 60);
  settings.setValue("break/small-for", smallBreakForSlider->value());
  settings.setValue("break/big-after", bigBreakAfterSlider->value());
  settings.setValue("break/big-for", bigBreakForSlider->value());
}

void PreferenceWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  QMainWindow::closeEvent(event);
}
