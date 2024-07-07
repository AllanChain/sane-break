#include "window.h"

#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>
#include <QSlider>
#include <QVBoxLayout>

#include "preferences/default.h"

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

  QLabel *imageLabel = new QLabel();
  QPixmap pixmap = QPixmap(":/images/icon@192.png");
  pixmap.setDevicePixelRatio(2.0);
  imageLabel->setPixmap(pixmap);
  imageLabel->setAlignment(Qt::AlignCenter);
  imageLabel->setMargin(20);
  layout->addWidget(imageLabel);

  QLabel *titleSmall = new QLabel("Small Break");
  QFont titleFont = titleSmall->font();
  titleFont.setBold(true);
  if (titleFont.pixelSize() > 0) {
    titleFont.setPixelSize(titleFont.pixelSize() * 1.5);
  } else {
    titleFont.setPointSize(titleFont.pointSize() * 1.5);
  }
  titleSmall->setFont(titleFont);
  layout->addWidget(titleSmall);

  QGridLayout *smallBreakForm = new QGridLayout();
  layout->addLayout(smallBreakForm);

  smallBreakForm->addWidget(new QLabel("Every"), 0, 0);
  smallBreakForm->addWidget(new QLabel("For"), 1, 0);

  smallBreakEverySlider = new QSlider(Qt::Horizontal);
  smallBreakEverySlider->setMaximum(60);
  smallBreakEverySlider->setTickPosition(QSlider::TicksBelow);
  smallBreakForm->addWidget(smallBreakEverySlider, 0, 1);

  smallBreakEveryLabel = new QLabel();
  smallBreakForm->addWidget(smallBreakEveryLabel, 0, 2);
  connect(smallBreakEverySlider, &QSlider::valueChanged, [this](int value) {
    smallBreakEveryLabel->setText(QString("%1 min").arg(value));
  });

  smallBreakForSlider = new QSlider(Qt::Horizontal);
  smallBreakForSlider->setMaximum(60);
  smallBreakForSlider->setTickPosition(QSlider::TicksBelow);
  smallBreakForm->addWidget(smallBreakForSlider, 1, 1);

  smallBreakForLabel = new QLabel();
  smallBreakForm->addWidget(smallBreakForLabel, 1, 2);
  connect(smallBreakForSlider, &QSlider::valueChanged, [this](int value) {
    smallBreakForLabel->setText(QString("%1 sec").arg(value));
  });

  QLabel *titleBig = new QLabel("Big Break");
  titleBig->setFont(titleFont);
  layout->addWidget(titleBig);

  QGridLayout *bigBreakForm = new QGridLayout();
  layout->addLayout(bigBreakForm);

  bigBreakForm->addWidget(new QLabel("After"), 0, 0);
  bigBreakForm->addWidget(new QLabel("For"), 1, 0);

  bigBreakAfterSlider = new QSlider(Qt::Horizontal);
  bigBreakAfterSlider->setMaximum(20);
  bigBreakAfterSlider->setTickPosition(QSlider::TicksBelow);
  bigBreakForm->addWidget(bigBreakAfterSlider, 0, 1);

  bigBreakAfterLabel = new QLabel();
  bigBreakForm->addWidget(bigBreakAfterLabel, 0, 2);
  connect(bigBreakAfterSlider, &QSlider::valueChanged, [this](int value) {
    bigBreakAfterLabel->setText(QString("%1 small breaks").arg(value));
  });

  bigBreakForSlider = new QSlider(Qt::Horizontal);
  bigBreakForSlider->setMaximum(300);
  bigBreakForSlider->setSingleStep(10);
  bigBreakForSlider->setTickInterval(60);
  bigBreakForSlider->setPageStep(60);
  bigBreakForSlider->setTickPosition(QSlider::TicksBelow);
  bigBreakForm->addWidget(bigBreakForSlider, 1, 1);

  bigBreakForLabel = new QLabel();
  bigBreakForm->addWidget(bigBreakForLabel, 1, 2);
  connect(bigBreakForSlider, &QSlider::valueChanged, [this](int value) {
    bigBreakForLabel->setText(QString("%1 sec").arg(value));
  });
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
