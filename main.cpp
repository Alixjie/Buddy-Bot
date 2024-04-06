#include <QWidget>
#include <QPainter>
#include <QApplication>

#include "sl_lidar.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <iomanip>
 
class MyWidget : public QWidget {
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setPen(Qt::blue); // 设置画笔颜色为蓝色
        painter.setBrush(Qt::blue); // 设置画刷颜色为蓝色
        painter.drawPoint(50, 50); // 在位置(50, 50)画一个点
    }
};
 
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);


    sl::ILidarDriver *lidar = *sl::createLidarDriver();
    if (!lidar) {
        std::cerr << "Insufficent memory, exit" << std::endl;
        exit(-1);
    }

    sl_lidar_response_device_health_t health_info;
    sl_lidar_response_device_info_t dev_info;
    do {
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

        std::cout << "Version: " << (int)dev_info.hardware_version << std::endl;

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

        } else {
            std::cerr << "Error, cannot retrieve the lidar health code: " << operat_result << std::endl;
            break;
        }

        if (health_info.status == SL_LIDAR_STATUS_ERROR) {
            std::cerr << "Error, slamtec lidar internal error detected. Please reboot the device to retry" << std::endl;
            lidar->reset();
            break;
        }
    } while(0);
 
    MyWidget widget;
    widget.show();
 
    return a.exec();
}
