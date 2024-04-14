#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>

#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <iomanip>
#include <cmath>

#include "sl_lidar.h"
#include "AStar.h"
#include "follow/ulm3_samples.h"
#include "VoiceControl/stt.h"

#define lidar_scan 1
#define obstacle_avoidance 2

#define test_x 128
#define test_y 72

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void paintEvent(QPaintEvent *event);

    void timerEvent(QTimerEvent *event);

    sl_result get_point_info_one_cycle(sl::ILidarDriver *drv);

    int get_lidar_point_info();

    int get_uwb_point_info();

    int uwb_initialise();

public slots:
    void start_lidar();

    void end_lidar();

    void start_obstacle_avoidance();

    void end_obstacle_avoidance();

    void voice_control_main();

private:
    Ui::MainWindow *ui;

    AStar star;

    ULM3Samples* ulm3_samples;
    output_data uwb_loc;

    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count;
    sl::ILidarDriver *lidar;

    QPoint origin_point;
    QPoint uwb_label_position;

    int num_paint;
    int timer_id_lidar_scan;
    int timer_id_obstacle_avoidance;

    int system_status;
};
#endif // MAINWINDOW_H
