#include <QWidget>
#include <QPainter>
#include <QApplication>

#include "mainwindow.h"
 
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
 
    MyWidget widget;
    widget.show();
 
    return a.exec();
}
