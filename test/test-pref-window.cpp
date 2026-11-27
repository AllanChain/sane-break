// Sane Break is a gentle break reminder that helps you avoid mindlessly skipping breaks
// Copyright (C) 2026 Sane Break developers
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QSettings>
#include <QSignalSpy>
#include <QSpinBox>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTest>
#include <QTimer>
#include <memory>

#include "app/break-window.h"
#include "app/pref-controller.h"
#include "app/pref-window.h"
#include "config.h"
#include "core/preferences.h"
#include "lib/hook-runner.h"

class TestPrefWindow : public QObject {
  Q_OBJECT

 private:
  SanePreferences* makePreferences(QTemporaryFile& tempFile, QObject* parent) {
    if (!tempFile.open()) qFatal("Failed to open temporary preferences file");
    return new SanePreferences(
        new QSettings(tempFile.fileName(), QSettings::IniFormat, parent), parent);
  }

  int openBreakWindowCount() {
    int count = 0;
    for (QWidget* w : QApplication::topLevelWidgets()) {
      if (qobject_cast<BreakWindow*>(w) && w->isVisible()) count++;
    }
    return count;
  }

 private slots:
  void controller_holder_owns_and_destroys_controllers() {
    QTemporaryFile tempFile;
    QObject root;
    SanePreferences* prefs = makePreferences(tempFile, &root);

    QCheckBox checkBox;
    QSpinBox spinBox;
    auto holder = std::make_unique<ControllerHolder>();
    QPointer<PrefControllerBase> checkCtrl = holder->add(
        PrefGroup::Schedule,
        new PrefController<QCheckBox, bool>(&checkBox, prefs->bigBreakEnabled));
    QPointer<PrefControllerBase> spinCtrl =
        holder->add(PrefGroup::Schedule,
                    new PrefController<QSpinBox, int>(&spinBox, prefs->smallFor));

    QCOMPARE(checkCtrl->parent(), holder.get());
    QCOMPARE(spinCtrl->parent(), holder.get());

    holder.reset();
    QVERIFY(checkCtrl.isNull());
    QVERIFY(spinCtrl.isNull());
  }

  void controller_holder_tracks_dirty_save_and_reload() {
    QTemporaryFile tempFile;
    QObject root;
    SanePreferences* prefs = makePreferences(tempFile, &root);
    prefs->smallFor->set(20);

    QSpinBox spinBox;
    spinBox.setRange(1, 300);
    ControllerHolder holder;
    holder.add(PrefGroup::Schedule,
               new PrefController<QSpinBox, int>(&spinBox, prefs->smallFor));
    QSignalSpy dirtySpy(&holder, &ControllerHolder::dirtyChanged);

    holder.load();
    QCOMPARE(spinBox.value(), 20);
    QVERIFY(!holder.isDirty);

    spinBox.setValue(35);
    QVERIFY(holder.isDirty);
    QCOMPARE(dirtySpy.count(), 1);
    QCOMPARE(dirtySpy.last().at(0).toBool(), true);
    QCOMPARE(prefs->smallFor->get(), 20);

    holder.reloadDirty();
    QCOMPARE(spinBox.value(), 20);
    QVERIFY(!holder.isDirty);

    spinBox.setValue(45);
    QVERIFY(holder.isDirty);
    holder.save();
    QVERIFY(!holder.isDirty);
    QCOMPARE(prefs->smallFor->get(), 45);
  }

  void preference_window_destroys_all_controllers() {
    QTemporaryFile tempFile;
    QObject root;
    SanePreferences* prefs = makePreferences(tempFile, &root);

    auto window = std::make_unique<PreferenceWindow>(prefs);
    QList<PrefControllerBase*> rawControllers =
        window->findChildren<PrefControllerBase*>();
    QVERIFY(rawControllers.size() >= 40);

    QList<QPointer<PrefControllerBase>> tracked;
    for (PrefControllerBase* ctrl : rawControllers) {
      QVERIFY(ctrl->parent() != nullptr);
      tracked.append(ctrl);
    }

    window.reset();
    for (const auto& ctrl : tracked) {
      QVERIFY(ctrl.isNull());
    }
  }

  void sync_big_break_enabled_toggles_all_four_tabs() {
    QTemporaryFile tempFile;
    QObject root;
    SanePreferences* prefs = makePreferences(tempFile, &root);

    PreferenceWindow window(prefs);
    window.show();
    QCoreApplication::processEvents();

    auto* bigBreakEnabled = window.findChild<QCheckBox*>("bigBreakEnabled");
    auto* bigBreakAfterBox = window.findChild<QSpinBox*>("bigBreakAfterBox");
    auto* autoCloseBig = window.findChild<QCheckBox*>("autoCloseWindowAfterBigBreak");
    auto* bigBgColor = window.findChild<QLineEdit*>("bigBackgroundColor");
    auto* bigShowClock = window.findChild<QCheckBox*>("bigBreakShowClock");
    auto* bigStartBell = window.findChild<QComboBox*>("bigStartBellSelect");
    QVERIFY(bigBreakEnabled && bigBreakAfterBox && autoCloseBig && bigBgColor &&
            bigShowClock && bigStartBell);

    bigBreakEnabled->setChecked(false);
    QVERIFY(!bigBreakAfterBox->isEnabled());
    QVERIFY(!autoCloseBig->isEnabled());
    QVERIFY(!bigBgColor->isEnabled());
    QVERIFY(!bigShowClock->isEnabled());
    QVERIFY(!bigStartBell->isEnabled());

    bigBreakEnabled->setChecked(true);
    QVERIFY(bigBreakAfterBox->isEnabled());
    QVERIFY(autoCloseBig->isEnabled());
    QVERIFY(bigBgColor->isEnabled());
    QVERIFY(bigShowClock->isEnabled());
    QVERIFY(bigStartBell->isEnabled());
  }

  void preview_cancels_on_reclick_close_and_destroy() {
    QTemporaryFile tempFile;
    QObject root;
    SanePreferences* prefs = makePreferences(tempFile, &root);

    auto window = std::make_unique<PreferenceWindow>(prefs);
    window->show();
    QCoreApplication::processEvents();

    auto* previewButton = window->findChild<QPushButton*>("previewButton");
    QVERIFY(previewButton != nullptr);
    int screenCount = QGuiApplication::screens().size();

    // Re-clicking Preview destroys the previous preview instead of stacking windows.
    emit previewButton->pressed();
    QCoreApplication::processEvents();
    QCOMPARE(openBreakWindowCount(), screenCount);

    emit previewButton->pressed();
    QCoreApplication::processEvents();
    QCOMPARE(openBreakWindowCount(), screenCount);

    // Closing PreferenceWindow stops the preview and closes break windows.
    window->close();
    QCoreApplication::processEvents();
    QCOMPARE(openBreakWindowCount(), 0);

    // Destroying PreferenceWindow mid-preview cleans up safely with no dangling timers.
    window->show();
    QCoreApplication::processEvents();
    emit previewButton->pressed();
    QCoreApplication::processEvents();
    QCOMPARE(openBreakWindowCount(), screenCount);

    window.reset();
    QCoreApplication::processEvents();
    QCOMPARE(openBreakWindowCount(), 0);
  }

  void hook_script_pref_and_runner() {
    QTemporaryFile tempFile;
    QObject root;
    SanePreferences* prefs = makePreferences(tempFile, &root);

    PreferenceWindow window(prefs);
    window.show();
    QCoreApplication::processEvents();

    auto* hookScriptPath = window.findChild<QLineEdit*>("hookScriptPath");
    auto* clearHookScript = window.findChild<QPushButton*>("clearHookScript");
    auto* saveButton = window.findChild<QPushButton*>("saveButton");
    QVERIFY(hookScriptPath && clearHookScript && saveButton);

#if defined(LINUX_DIST_FLATPAK) || defined(SANE_APP_STORE)
    QVERIFY(!hookScriptPath->isVisible());
#else
    hookScriptPath->setText("/usr/bin/true");
    emit saveButton->pressed();
    QCOMPARE(prefs->hookScript->get(), QString("/usr/bin/true"));

    emit clearHookScript->pressed();
    emit saveButton->pressed();
    QCOMPARE(prefs->hookScript->get(), QString(""));

    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QString outPath = tempDir.filePath("out.txt");
    QString scriptPath = tempDir.filePath("hook.sh");

    QFile scriptFile(scriptPath);
    QVERIFY(scriptFile.open(QIODevice::WriteOnly | QIODevice::Text));
    scriptFile.write("#!/bin/sh\n");
    scriptFile.write(
        "printf '%s|%s|%s' \"$1\" \"$SANE_BREAK_EVENT\" "
        "\"$SANE_BREAK_TYPE\" > \"$SANE_TEST_OUT\"\n");
    scriptFile.close();
    QVERIFY(QFile::setPermissions(scriptPath, QFileDevice::ReadOwner |
                                                  QFileDevice::WriteOwner |
                                                  QFileDevice::ExeOwner));

    runHookScript(scriptPath, "break-fullscreen",
                  {{"SANE_BREAK_TYPE", "big"}, {"SANE_TEST_OUT", outPath}});

    QTRY_VERIFY_WITH_TIMEOUT(QFileInfo(outPath).size() > 0, 2000);
    QFile resultFile(outPath);
    QVERIFY(resultFile.open(QIODevice::ReadOnly));
    QCOMPARE(QString::fromUtf8(resultFile.readAll()),
             QString("break-fullscreen|break-fullscreen|big"));
#endif
  }
};

QTEST_MAIN(TestPrefWindow)
#include "test-pref-window.moc"
