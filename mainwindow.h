#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>

#include "sl_lidar.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <iomanip>
#include <cmath>

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
    void timerEvent(QTimerEvent *event);

    sl_result get_point_info_one_cycle(sl::ILidarDriver *drv);
    int get_lidar_point_info();

public slots:
    void start_lidar();
    void end_lidar();

private:
    Ui::MainWindow *ui;
    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count;
    sl::ILidarDriver *lidar;
    int num_paint;
    int timer_id;
};
#endif // MAINWINDOW_H
