#include <QWidget>
#include <QPainter>
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow main_window;
//    main_window.showFullScreen();
    main_window.show();

    return a.exec();
}
