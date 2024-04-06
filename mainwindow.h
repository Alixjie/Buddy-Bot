#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sl_lidar.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <iomanip>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent *event);
    sl_result get_point_info_one_cycle(sl::ILidarDriver *drv);

public:
    int get_lidar_point_info();

private:
    Ui::MainWindow *ui;
    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count;
};
#endif // MAINWINDOW_H
