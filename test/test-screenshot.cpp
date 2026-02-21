// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QDir>
#include <QFontDatabase>
#include <QObject>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QString>
#include <QTemporaryFile>
#include <QTest>
#include <QWidget>
#include <Qt>
#include <QtEnvironmentVariables>

#include "app/break-window.h"
#include "app/break-windows.h"
#include "app/focus-window.h"
#include "app/meeting-window.h"
#include "app/postpone-window.h"
#include "app/pref-window.h"
#include "app/stats-window.h"
#include "app/welcome.h"
#include "core/break-windows.h"
#include "core/db.h"
#include "core/flags.h"
#include "core/preferences.h"

static SanePreferences* tempPreferences() {
  QTemporaryFile tempFile;
  (void)tempFile.open();
  return new SanePreferences(new QSettings(tempFile.fileName(), QSettings::IniFormat));
}

static QString screenshotDir() {
  QString dir = qEnvironmentVariable("SCREENSHOT_DIR");
  if (dir.isEmpty()) dir = QDir::currentPath() + "/screenshots";
  QDir().mkpath(dir);
  return dir;
}

static void saveScreenshot(QWidget* widget, const QString& name) {
  QPixmap pixmap = widget->grab();
  QString path = screenshotDir() + "/" + name + ".png";
  QVERIFY2(pixmap.save(path), qPrintable("Failed to save screenshot: " + path));
  qDebug() << "Saved:" << path << pixmap.size();
}

class TestScreenshot : public QObject {
  Q_OBJECT
 private:
  SanePreferences* prefs = nullptr;
  QSqlDatabase sqlDb;
  BreakDatabase* db = nullptr;

 private slots:
  void initTestCase() {
    QFontDatabase::addApplicationFont(":/fonts/bootstrap-icons.ttf");
    prefs = tempPreferences();
    sqlDb = QSqlDatabase::addDatabase("QSQLITE", "screenshot-db");
    sqlDb.setDatabaseName(":memory:");
    db = new BreakDatabase(sqlDb);
    db->logEvent("test::init");
    populateSampleData();
  }

  void cleanupTestCase() {
    delete db;
    delete prefs;
    sqlDb = QSqlDatabase();
    QSqlDatabase::removeDatabase("screenshot-db");
  }

  void breakWindow() {
    BreakWindowData data = BreakWindows::createData(BreakType::Big, prefs);
    data.show.prograssBar = true;
    data.show.countdown = true;
    data.show.clock = true;
    data.show.endTime = true;
    data.show.buttons = true;
    BreakWindow window(data);
    window.setGeometry(0, 0, 800, 600);
    window.show();
    QApplication::processEvents();
    window.showFullScreen();
    window.setTime(45, "14:30:00");
    window.setClock("14:29");
    window.showButtons(AbstractBreakWindows::Button::LockScreen |
                       AbstractBreakWindows::Button::ExitForceBreak);
    QApplication::processEvents();
    saveScreenshot(&window, "break-window");
  }

  void preferenceWindow_data() {
    QTest::addColumn<int>("tabIndex");
    QTest::addColumn<QString>("tabButtonName");
    QTest::addColumn<QString>("screenshotName");

    QTest::newRow("schedule") << 0 << "tabScheduleButton" << "pref-schedule";
    QTest::newRow("reminder") << 1 << "tabReminderButton" << "pref-reminder";
    QTest::newRow("interface") << 2 << "tabInterfaceButton" << "pref-interface";
    QTest::newRow("pause") << 3 << "tabPauseButton" << "pref-pause";
    QTest::newRow("sound") << 4 << "tabSoundButton" << "pref-sound";
    QTest::newRow("general") << 5 << "tabGeneralButton" << "pref-general";
    QTest::newRow("about") << 6 << "tabAboutButton" << "pref-about";
  }

  void preferenceWindow() {
    QFETCH(QString, tabButtonName);
    QFETCH(QString, screenshotName);

    PreferenceWindow window(prefs);
    window.show();
    QApplication::processEvents();

    // Click the tab button to switch tabs
    auto* button = window.findChild<QPushButton*>(tabButtonName);
    QVERIFY2(button, qPrintable("Tab button not found: " + tabButtonName));
    button->click();
    QApplication::processEvents();

    // Resize window height to fit all tab content without scrolling.
    // The stacked widget has a fixed height matching current page's sizeHint.
    // We resize the window tall enough so the scroll area doesn't clip.
    auto* scrollArea = window.findChild<QScrollArea*>("scrollArea");
    QVERIFY(scrollArea);
    int contentHeight = scrollArea->widget()->sizeHint().height();
    // Add margins for menu bar, scroll area frame, and some padding
    int totalHeight = contentHeight + 40;
    window.resize(700, totalHeight);
    QApplication::processEvents();

    saveScreenshot(&window, screenshotName);
  }

  void welcomeWindow() {
    WelcomeWindow window(prefs);
    window.show();
    QApplication::processEvents();
    saveScreenshot(&window, "welcome-window");
  }

  void meetingWindow() {
    auto* window = new MeetingWindow(prefs, db);
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "meeting-window");
    delete window;
  }

  void postponeWindow() {
    auto* window = new PostponeWindow(prefs, db);
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "postpone-window");
    delete window;
  }

  void focusWindow() {
    auto* window = new FocusWindow(prefs);
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "focus-window");
    delete window;
  }

  void statsWindow() {
    auto* window = new StatsWindow(db);
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->resize(700, 500);
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "stats-window");
    delete window;
  }

 private:
  void populateSampleData() {
    QDate today = QDate::currentDate();
    for (int i = 6; i >= 0; i--) {
      QDate day = today.addDays(-i);
      QString d = day.toString(Qt::ISODate);
      // Morning work session
      insertSpan("normal", d + " 09:00:00", d + " 12:00:00");
      // Small break
      insertSpan("break", d + " 12:00:00", d + " 12:00:20",
                 R"({"type":"small","normal-exit":true})");
      // Afternoon work session
      insertSpan("normal", d + " 13:00:00", d + " 17:00:00");
      // Big break
      insertSpan("break", d + " 15:00:00", d + " 15:05:00",
                 R"({"type":"big","normal-exit":true})");
    }
  }

  void insertSpan(const QString& type, const QString& startedAt, const QString& endedAt,
                  const QString& data = "{}") {
    QSqlQuery query(sqlDb);
    query.prepare(
        "INSERT INTO spans (type, started_at, ended_at, data) "
        "VALUES (?, ?, ?, ?)");
    query.addBindValue(type);
    query.addBindValue(startedAt);
    query.addBindValue(endedAt);
    query.addBindValue(data);
    query.exec();
  }
};

QTEST_MAIN(TestScreenshot)
#include "test-screenshot.moc"
