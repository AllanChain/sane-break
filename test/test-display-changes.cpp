// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qpa/qplatformscreen.h>
#include <qpa/qwindowsysteminterface.h>

#include <QApplication>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <memory>

#include "app/break-windows.h"

// Exercise real QScreen lifecycle/geometry signals without physical displays.
class TestScreen : public QPlatformScreen {
 public:
  QRect rect{0, 0, 1280, 720};
  QRect geometry() const override { return rect; }
  int depth() const override { return 32; }
  QImage::Format format() const override { return QImage::Format_RGB32; }
  QString name() const override { return "display-change-test"; }
  void resizeTo(QRect geometry) {
    rect = geometry;
    QWindowSystemInterface::handleScreenGeometryChange(screen(), rect, rect);
    QCoreApplication::processEvents();
  }
};

class TestDisplayChanges : public QObject {
  Q_OBJECT

 private:
  QTemporaryDir settingsDir;
  std::unique_ptr<SanePreferences> prefs;
  std::unique_ptr<BreakWindows> windows;
  QList<TestScreen*> screens;

  TestScreen* addScreen() {
    auto* screen = new TestScreen;
    screens.append(screen);
    QWindowSystemInterface::handleScreenAdded(screen);
    return screen;
  }
  void removeScreen(TestScreen* screen) {
    screens.removeOne(screen);
    QWindowSystemInterface::handleScreenRemoved(screen);
  }
  template <typename T>
  T* windowOn(QScreen* screen) {
    for (QWidget* widget : QApplication::topLevelWidgets())
      if (auto* w = qobject_cast<T*>(widget);
          w && w->isVisible() && w->screenIdentity() == screen)
        return w;
    return nullptr;
  }

 private slots:
  void init() {
    prefs = std::make_unique<SanePreferences>(
        new QSettings(settingsDir.filePath("test.ini"), QSettings::IniFormat));
    prefs->smallBreakShowCountdown->set(true);
    prefs->smallBreakShowButtons->set(true);
    windows = std::make_unique<BreakWindows>();
  }
  void cleanup() {
    windows->destroy();
    windows->hideHeadsUp();
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    windows.reset();
    while (!screens.isEmpty()) removeScreen(screens.last());
    prefs.reset();
  }
  void replace_screen_then_change_resolution() {
    auto* oldScreen = addScreen();
    windows->create(BreakType::Small, prefs.get(), 60);
    windows->showFullScreen();
    windows->setTime(37);
    windows->showButtons(AbstractBreakWindows::Button::ExitForceBreak);
    QPointer<BreakWindow> oldWindow = windowOn<BreakWindow>(oldScreen->screen());
    QVERIFY(oldWindow);

    auto* replacement = addScreen();
    // The final resolution can arrive after the screen-added debounce expires.
    QTRY_VERIFY(windowOn<BreakWindow>(replacement->screen()));
    removeScreen(oldScreen);
    QVERIFY(!oldWindow || !oldWindow->isVisible());
    replacement->resizeTo(QRect(1280, 0, 1920, 1080));
    windows->setTime(36);  // countdown advances during display negotiation
    QTRY_VERIFY(windowOn<BreakWindow>(replacement->screen()));
    QTRY_COMPARE(windowOn<BreakWindow>(replacement->screen())->geometry(),
                 replacement->rect);
    auto* current = windowOn<BreakWindow>(replacement->screen());
    QCOMPARE(current->findChild<QLabel*>("countDownLabel")->text(), QString("36"));
    QVERIFY(current->centralWidget()->property("isFullScreen").toBool());
    QSignalSpy exit(windows.get(), &BreakWindows::exitForceBreakRequested);
    auto* button = current->findChild<QPushButton*>("exitForceBreak");
    QVERIFY(button->isVisible());
    button->click();
    QCOMPARE(exit.count(), 1);
    windows->setTime(35);
    QCOMPARE(current->findChild<QLabel*>("countDownLabel")->text(), QString("35"));
  }
  void resolution_only_preserves_prompt() {
    auto* screen = addScreen();
    windows->create(BreakType::Small, prefs.get(), 60);
    screen->resizeTo(QRect(1280, 100, 1600, 900));
    QTRY_VERIFY(windowOn<BreakWindow>(screen->screen()));
    QTRY_COMPARE(windowOn<BreakWindow>(screen->screen())->geometry(),
                 QRect(1880, 100, 400, 120));
    QVERIFY(!windowOn<BreakWindow>(screen->screen())
                 ->centralWidget()
                 ->property("isFullScreen")
                 .toBool());
  }
  void heads_up_follows_available_geometry() {
    auto* screen = addScreen();
    windows->showHeadsUp(30, BreakType::Small, prefs.get());
    windows->setHeadsUpTime(12);
    screen->resizeTo(QRect(1280, 100, 1600, 900));
    QTRY_VERIFY(windowOn<HeadsUpWindow>(screen->screen()));
    QTRY_COMPARE(windowOn<HeadsUpWindow>(screen->screen())->geometry(),
                 QRect(1940, 116, 280, 56));
    QVERIFY(windowOn<HeadsUpWindow>(screen->screen())->property("progress").toReal() <=
            0.4);
  }
  void pending_reconcile_cannot_resurrect_finished_break() {
    auto* screen = addScreen();
    windows->create(BreakType::Small, prefs.get(), 60);
    screen->resizeTo(QRect(0, 0, 1920, 1080));
    windows->destroy();
    QTest::qWait(900);
    QVERIFY(!windowOn<BreakWindow>(screen->screen()));
  }
};

QTEST_MAIN(TestDisplayChanges)
#include "test-display-changes.moc"
