// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFontDatabase>
#include <QLineEdit>
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
#include <QTime>
#include <QTimeZone>
#include <QWidget>
#include <Qt>
#include <QtEnvironmentVariables>

#include "app/break-window.h"
#include "app/break-windows.h"
#include "app/focus-window.h"
#include "app/heads-up-window.h"
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
  // Breeze draws a 1px titlebar-separator line at y=0 on decorated QDialog and
  // QMainWindow widgets; crop that top pixel on decorated windows.
  int topOffset = (widget->windowFlags() & Qt::FramelessWindowHint) ? 0 : 1;
  QPixmap pixmap = widget->grab(widget->rect().adjusted(0, topOffset, 0, 0));
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
    BreakWindowData data =
        BreakWindowData::create(BreakType::Big, prefs, prefs->bigFor->get());
    data.show.prograssBar = true;
    data.show.countdown = true;
    data.show.clock = true;
    data.show.endTime = true;
    data.show.buttons = true;
    BreakWindow window(data);
    window.initSize(QApplication::primaryScreen());
    window.showFullScreen(/*animate=*/false);
    window.setGeometry(0, 0, 800, 600);
    window.show();
    window.setTime(45, "14:30:00", /*animate=*/false);
    window.setClock("14:29");
    window.showButtons(AbstractBreakWindows::Button::LockScreen |
                       AbstractBreakWindows::Button::ExitForceBreak |
                       AbstractBreakWindows::Button::PauseMedia);
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
    window.resize(400, 460);
    QApplication::processEvents();
    saveScreenshot(&window, "welcome-window");
  }

  void meetingWindow() {
    auto* window = new MeetingWindow(prefs);
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    if (auto* reasonEdit = window->findChild<QLineEdit*>("reasonEdit")) {
      reasonEdit->setText("Weekly team sync");
      reasonEdit->clearFocus();
    }
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "meeting-window");
    delete window;
  }

  void postponeWindow() {
    auto* window = new PostponeWindow(prefs);
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->setMinutes(5);
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "postpone-window");
    delete window;
  }

  void focusWindow() {
    auto* window = new FocusWindow(prefs);
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    if (auto* reasonEdit = window->findChild<QLineEdit*>("reasonEdit")) {
      reasonEdit->setText("Writing documentation");
      reasonEdit->clearFocus();
    }
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "focus-window");
    delete window;
  }

  void headsUpWindow() {
    auto* window =
        new HeadsUpWindow(30, prefs->smallBackgroundColor->get(),
                          prefs->smallFlashColor->get(), prefs->messageColor->get());
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->setTime(20);
    window->show();
    QApplication::processEvents();
    saveScreenshot(window, "heads-up-window");
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

      // Morning session: 09:00 - 10:30 normal, break, 10:31 - 12:00 normal
      insertLocalSpan(day, "normal", "09:00:00", "10:20:00");
      insertLocalSpan(day, "break", "10:20:00", "10:20:20",
                      R"({"type":"small","normal-exit":true})");
      insertLocalSpan(day, "flash", "10:19:52", "10:20:00");
      insertLocalSpan(day, "normal", "10:20:20", "12:00:00");
      insertLocalSpan(day, "break", "12:00:00", "12:01:00",
                      R"({"type":"big","normal-exit":true})");
      insertLocalSpan(day, "flash", "11:59:50", "12:00:00");

      // Afternoon session with variety across days
      if (i % 3 == 0) {
        // Focus session in early afternoon + meeting later
        insertLocalSpan(day, "normal", "13:00:00", "14:20:00");
        insertLocalSpan(day, "focus", "13:00:00", "14:20:00",
                        R"({"totalCycles":4,"reason":"Deep work"})");
        insertLocalSpan(day, "break", "14:20:00", "14:21:00",
                        R"({"type":"big","normal-exit":true})");
        insertLocalSpan(day, "flash", "14:19:54", "14:20:00");
        insertLocalSpan(day, "normal", "14:21:00", "15:30:00");
        insertLocalSpan(day, "meeting", "15:30:00", "16:30:00",
                        R"({"scheduledSeconds":3600,"reason":"Team sync"})");
        insertLocalSpan(day, "normal", "16:30:00", "17:30:00");
        insertLocalEvent(day, "postpone", "15:00:00", R"({"seconds":300})");
      } else if (i % 3 == 1) {
        // Paused block in mid-afternoon
        insertLocalSpan(day, "normal", "13:15:00", "15:00:00");
        insertLocalSpan(day, "paused", "15:00:00", "15:45:00",
                        R"({"reasons":["on-battery"]})");
        insertLocalSpan(day, "normal", "15:45:00", "17:45:00");
        insertLocalSpan(day, "break", "17:45:00", "17:46:00",
                        R"({"type":"big","normal-exit":true})");
        insertLocalSpan(day, "flash", "17:44:53", "17:45:00");
      } else {
        // Meeting in early afternoon + focus block later
        insertLocalSpan(day, "meeting", "13:00:00", "14:00:00",
                        R"({"scheduledSeconds":3600,"reason":"Design review"})");
        insertLocalSpan(day, "normal", "14:00:00", "16:00:00");
        insertLocalSpan(day, "focus", "14:30:00", "15:50:00",
                        R"({"totalCycles":4,"reason":"Code review"})");
        insertLocalSpan(day, "break", "16:00:00", "16:01:00",
                        R"({"type":"big","normal-exit":true})");
        insertLocalSpan(day, "flash", "15:59:51", "16:00:00");
        insertLocalSpan(day, "normal", "16:01:00", "17:20:00");
        insertLocalEvent(day, "postpone", "16:45:00", R"({"seconds":300})");
      }
    }
  }

  static QString toUtcSqlString(QDate day, const QString& timeStr) {
    QDateTime localDt(day, QTime::fromString(timeStr, "HH:mm:ss"),
                      QTimeZone::systemTimeZone());
    return localDt.toUTC().toString("yyyy-MM-dd HH:mm:ss");
  }

  void insertLocalSpan(QDate day, const QString& type, const QString& startTime,
                       const QString& endTime, const QString& data = "{}") {
    QSqlQuery query(sqlDb);
    query.prepare(
        "INSERT INTO spans (type, started_at, ended_at, data) "
        "VALUES (?, ?, ?, ?)");
    query.addBindValue(type);
    query.addBindValue(toUtcSqlString(day, startTime));
    query.addBindValue(toUtcSqlString(day, endTime));
    query.addBindValue(data);
    query.exec();
  }

  void insertLocalEvent(QDate day, const QString& type, const QString& timeStr,
                        const QString& data = "{}") {
    QSqlQuery query(sqlDb);
    query.prepare(
        "INSERT INTO events (type, created_at, data) "
        "VALUES (?, ?, ?)");
    query.addBindValue(type);
    query.addBindValue(toUtcSqlString(day, timeStr));
    query.addBindValue(data);
    query.exec();
  }
};

QTEST_MAIN(TestScreenshot)
#include "test-screenshot.moc"
