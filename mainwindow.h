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

#include "rplidar/sl_lidar.h"
#include "rplidar/AStar.h"
#include "follow/ulm3_samples.h"
#include "motorControl/movement.hpp"
#include "motorControl/encoder.hpp"
#include "motorControl/pwm.hpp"
#include "motorControl/motorDriver.hpp"
#include "voicecontrol/voicecontrol.h"

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

    void voice_control_start();

    void voice_control_stop();

    void car_control_main();

    void car_control_stop();

    void car_come_here();

private:
    Ui::MainWindow *ui;

    AStar star;

    output_data uwb_loc;

    VoiceControl vc;

    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count;
    sl::ILidarDriver *lidar;

    QPoint origin_point;
    QPoint uwb_label_position;

    re_decoder *decR;
    re_decoder *decL;

    int num_paint;
    int timer_id_lidar_scan;
    int timer_id_obstacle_avoidance;

    int system_status;

    double mult;
};
#endif // MAINWINDOW_H
