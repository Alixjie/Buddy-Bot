#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifndef count_of
#define count_of(array) (int)(sizeof (array) / sizeof (array[0]))
#endif

static inline void delay(unsigned long sec) {
    while (sec >= 1000) {
        usleep(1000 * 1000);
        sec -= 1000;
    };
    if (sec != 0)
        usleep(sec * 1000);
}

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->start_lidar, &QPushButton::clicked, this, &MainWindow::start_lidar);
    connect(ui->end_lidar, &QPushButton::clicked, this, &MainWindow::end_lidar);

    connect(ui->start_obstacle_avoidance, &QPushButton::clicked, this, &MainWindow::start_obstacle_avoidance);
    connect(ui->end_obstacle_avoidance, &QPushButton::clicked, this, &MainWindow::end_obstacle_avoidance);

    connect(ui->quit_program, &QPushButton::clicked, this, &QApplication::quit);

    connect(ui->voice_control, &QPushButton::clicked, this, &MainWindow::voice_control_main);
    connect(ui->follow_me, &QPushButton::clicked, this, &MainWindow::car_control_main);

    char pname[]="/dev/ttyUSB1";
    ulm3_samples = new ULM3Samples(pname);
    ulm3_samples->start();

    if (gpioInitialise() < 0)
        exit(-2);
    Set_Pwm();
    decR = new re_decoder(16, 12, callbackR);
    decL = new re_decoder(20, 21, callbackL);

    //Periodically calling timercallback to change the motion status and keep speed-loop.
    gpioSetTimerFunc(0, 54, timercallback);

    mult = 0.5;
    lidar = *sl::createLidarDriver();
    if (!lidar) {
        std::cerr << "Insufficent memory, exit" << std::endl;
        exit(-1);
    }

    sl_lidar_response_device_health_t health_info;
    sl_lidar_response_device_info_t dev_info;

    sl::IChannel *channel = *sl::createSerialPortChannel("/dev/ttyUSB0", 460800);

    if (SL_IS_FAIL(lidar->connect(channel)))
        std::cerr << "Error, cannot bind to the specified serial port /dev/ttyUSB0" << std::endl;

    sl_result operat_result = lidar->getDeviceInfo(dev_info);

    if (SL_IS_FAIL(operat_result)) {
        if (operat_result == SL_RESULT_OPERATION_TIMEOUT)
            std::cerr << "Error, operation time out" << std::endl;
        else
            std::cerr << "Error, unexpected error, code: " << operat_result << std::endl;
    }

    std::cout << "Version: " << (int) dev_info.hardware_version << std::endl;

    operat_result = lidar->getHealth(health_info);
    if (SL_IS_OK(operat_result)) {
        std::cout << "Lidar health status : ";
        switch (health_info.status) {
            case SL_LIDAR_STATUS_OK:
                std::cout << "OK!";
                break;
            case SL_LIDAR_STATUS_WARNING:
                std::cout << "Warning!";
                break;
            case SL_LIDAR_STATUS_ERROR:
                std::cout << "Error!";
                break;
        }
        std::cerr << std::endl << "errorcode: " << health_info.error_code << std::endl;

    } else
        std::cerr << "Error, cannot retrieve the lidar health code: " << operat_result << std::endl;


    if (health_info.status == SL_LIDAR_STATUS_ERROR) {
        std::cerr << "Error, slamtec lidar internal error detected. Please reboot the device to retry" << std::endl;
        lidar->reset();
    }

    lidar->setMotorSpeed();

    if (SL_IS_FAIL(lidar->startScan(0, 1))) {
        std::cerr << "Error, cannot start the scan operation" << std::endl;
    }

    num_paint = 0;
    delay(3000);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    if (num_paint > 10) {
        QPainter painter(this);
        QPen pen1(Qt::blue, 8, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen1);

        if (system_status == lidar_scan) {
            painter.drawPoint(origin_point);

            QPen pen2(Qt::red, 6, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen2);

            for (int num = 0; num < (int) count; ++num) {
//        std::cout << "Theta: " << std::fixed << std::setprecision(2) << nodes[num].angle_z_q14 * 90.f / 16384.f;
//        std::cout << "  Distance: " << nodes[num].dist_mm_q2 / 4.0f;
//        std::cout << "  Quality: " << (nodes[num].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) << std::endl;

                if ((nodes[num].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) != 0) {
                    double x = origin_point.x() +
                               (nodes[num].dist_mm_q2 / 4.0f) *
                               sin((nodes[num].angle_z_q14 * 90.f / 16384.f) * 3.14 / 180);
                    double y = origin_point.y() -
                               (nodes[num].dist_mm_q2 / 4.0f) *
                               cos((nodes[num].angle_z_q14 * 90.f / 16384.f) * 3.14 / 180);

                    painter.drawPoint(QPoint(int(x), int(y)));
                }
            }
        }

        if (system_status == obstacle_avoidance) {
            painter.drawPoint(origin_point);
            painter.drawPoint(uwb_label_position);

            QPen pen2(Qt::red, 6, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen2);

            for (int num = 0; num < (int) count; ++num) {
//        std::cout << "Theta: " << std::fixed << std::setprecision(2) << nodes[num].angle_z_q14 * 90.f / 16384.f;
//        std::cout << "  Distance: " << nodes[num].dist_mm_q2 / 4.0f;
//        std::cout << "  Quality: " << (nodes[num].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) << std::endl;

                if (((nodes[num].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) != 0) &&
                    (((nodes[num].angle_z_q14 * 90.f / 16384.f) <= 110.f) ||
                     ((nodes[num].angle_z_q14 * 90.f / 16384.f) >= 250.f))) {
                    double x = origin_point.x() + mult * (
                               (nodes[num].dist_mm_q2 / 4.0f) *
                               sin((nodes[num].angle_z_q14 * 90.f / 16384.f) * 3.14 / 180));
                    double y = origin_point.y() - mult * (
                               (nodes[num].dist_mm_q2 / 4.0f) *
                               cos((nodes[num].angle_z_q14 * 90.f / 16384.f) * 3.14 / 180));

//                    std::cout << "Point x y : " << x << ", " << y << " " << (nodes[num].angle_z_q14 * 90.f / 16384.f) << " ";
                    painter.drawPoint(QPoint(int(x), int(y)));
                }
            }

            std::list <Point> &returnList = star.getReturnList();
//            std::cout << "The way is" << " (" << returnList.size() << "): ";
//            int **map = star.getMap();

//            for (int i = 0; i < test_x;uwb_initialise() ++i) {
//                for (int j = 0; j < test_y; ++j)
//                    std::cout << map[i][j] << " ";
//                std::cout << std::endl;
//            }
            QPen pen3(Qt::green, 6, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen3);

            for (std::list<Point>::iterator it = returnList.begin(); it != returnList.end(); ++it) {
                painter.drawPoint(QPoint(int(it->x) * 10, int(it->y) * 10));
//                if ((it->x == star.getEndPoint().x) && (it->y == star.getEndPoint().y))
//                    std::cout << "( " << it->x << ", " << it->y << " )";
//                else
//                    std::cout << "( " << it->x << ", " << it->y << " )" << " -> ";
            }
        }
    }
    ++num_paint;
}


int MainWindow::get_uwb_point_info() {
    uwb_loc = ulm3_samples->getData();
    uwb_label_position = QPoint(500 + uwb_loc.x * 6, 700 - uwb_loc.y * 6);
    std::cout << uwb_label_position.x() <<',' << uwb_label_position.y()<<std::endl;

//    uwb_label_position = QPoint(500, 100);
    return 0;
}

int MainWindow::uwb_initialise()
{
//    char pname[] = "/dev/ttyUSB1";
//    ulm3_samples = ULM3Samples(pname);
    //ulm3_samples.start();
}


void MainWindow::timerEvent(QTimerEvent *event) {
    if (system_status == lidar_scan) {
        get_lidar_point_info();
        origin_point = QPoint(500, 500);
        update();
    } else {
        get_lidar_point_info();
        get_uwb_point_info();

        star = AStar(test_x, test_y);
        star.setStartPoint(Point(50, 70));
        origin_point = QPoint(500, 700);
        star.setEndPoint(Point(int(uwb_label_position.x() / 10), int(uwb_label_position.y() / 10)));

        for (int num = 0; num < (int) count; ++num) {
            if (((nodes[num].quality >> SL_LIDAR_RESP_MEASUREMENT_QUALITY_SHIFT) != 0) &&
                (((nodes[num].angle_z_q14 * 90.f / 16384.f) <= 110.f) ||
                 ((nodes[num].angle_z_q14 * 90.f / 16384.f) >= 250.f))) {
                double x = origin_point.x() +
                           (nodes[num].dist_mm_q2 / 4.0f) *
                           sin((nodes[num].angle_z_q14 * 90.f / 16384.f) * 3.14 / 180);
                double y = origin_point.y() -
                           (nodes[num].dist_mm_q2 / 4.0f) *
                           cos((nodes[num].angle_z_q14 * 90.f / 16384.f) * 3.14 / 180);

//                star.changeBlockState(int(x / 10), int(y / 10));
//                std::cout << "x, y: " << x << ", " << y << " ";
//                std::cout << "int (x, y): " << int(x / 10) << " " << int(y / 10) << " ";
                int x_start = int(x / 10) - 2;
                int y_start = int(y / 10) - 2;

                for (int i = x_start; i <= x_start + 2; ++i)
                    for (int j = y_start; j <= y_start + 2; ++j) {
                        star.changeBlockState(i, j);
//                        std::cout << "x, y: " << x << ", " << y << " ";
//                        std::cout << "i, j: " << i << ", " << j << " ";
                    }
            }
        }
        if (star.totalLogic())
            std::cout << "Can arrive" << std::endl;
        else
            std::cout << "Can't arrive" << std::endl;

        update();
    }
}

sl_result MainWindow::get_point_info_one_cycle(sl::ILidarDriver *drv) {
    count = count_of(nodes);
//    std::cout << "Count before getting the data: " << count << std::endl;

    std::cout << "waiting for data..." << std::endl;

    sl_result result = drv->grabScanDataHq(nodes, count, 0);
    std::cout << "Count after getting the data: " << count << std::endl;
    if (SL_IS_OK(result) || result == SL_RESULT_OPERATION_TIMEOUT)
        drv->ascendScanData(nodes, count);
    else
        std::cerr << "error code: " << result << std::endl;
    return result;
}

int MainWindow::get_lidar_point_info() {
    if (SL_IS_FAIL(get_point_info_one_cycle(lidar)))
        std::cerr << "Error, cannot grab scan data" << std::endl;

    return 0;
}

void MainWindow::start_lidar() {
    system_status = lidar_scan;
    timer_id_lidar_scan = startTimer(110);
}

void MainWindow::end_lidar() {
    killTimer(timer_id_lidar_scan);
}

void MainWindow::start_obstacle_avoidance() {
    system_status = obstacle_avoidance;
    timer_id_obstacle_avoidance = startTimer(500);
}

void MainWindow::end_obstacle_avoidance() {
    killTimer(timer_id_obstacle_avoidance);
}

void MainWindow::voice_control_main()
{
    voice_control();
}

void MainWindow::car_control_main()
{
    ulm3_samples->controlCar();
}

void MainWindow::car_come_here()
{
    MoveControll& mc = MoveControll::getInstance();
    std::list <Point> &returnList = star.getReturnList();

    for (std::list<Point>::iterator it = returnList.begin(); it != returnList.end(); ++it) {
        int degree = atan(it->y / it->x);
        mc.SetFromAngel(degree);
        mc.sem.wait();
        int distance = sqrt(it->y * it->y + it->x * it->x);
        mc.SetFromDistance(distance);
        mc.sem.wait();
//                if ((it->x == star.getEndPoint().x) && (it->y == star.getEndPoint().y))
//                    std::cout << "( " << it->x << ", " << it->y << " )";
//                else
//                    std::cout << "( " << it->x << ", " << it->y << " )" << " -> ";
    }
}

MainWindow::~MainWindow() {
    lidar->stop();
    delay(20);
    lidar->setMotorSpeed(0);

    if(ulm3_samples){
        delete ulm3_samples;
        ulm3_samples = nullptr;
    }

    gpioSetTimerFunc(0, 0, nullptr);
    clearALL();
    decL->re_cancel();
    decR->re_cancel();
    gpioTerminate();

    if (lidar) {
        delete lidar;
        lidar = nullptr;
    }
    delete ui;
}

