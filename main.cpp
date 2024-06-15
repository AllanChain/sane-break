#include "mainwindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QList<QScreen*> screens = QApplication::screens();

    for (QScreen *screen : screens) {
        MainWindow *w = new MainWindow();
        w->setGeometry(screen->availableGeometry());
        w->showFullScreen();
        w->show();
    }


    return a.exec();
}
