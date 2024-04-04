//
// Created by George on 2024/4/4.
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <iomanip>

#include "sl_lidar.h"
#include "sl_lidar_driver.h"

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

sl_result get_point_info_one_cycle(sl::ILidarDriver *drv) {
    sl_lidar_response_measurement_node_hq_t nodes[8192];
    size_t count = count_of(nodes);

    std::cout << "waiting for data..." << std::endl;

    sl_result result = drv->grabScanDataHq(nodes, count, 0);
    if (SL_IS_OK(result) || result == SL_RESULT_OPERATION_TIMEOUT) {
        drv->ascendScanData(nodes, count);

        for (int num = 0; num < (int) count; ++num) {
            std::cout << "Theta: " << std::setprecision(4) << nodes[num].angle_z_q14 * 90.f / 16384.f;
            std::cout << "Distance: " << nodes[num].dist_mm_q2 / 4.0f << std::endl;
        }
    } else
        std::cerr << "error code: " << result << std::endl;
    return result;
}

int main(int argc, const char *argv[]) {
    sl::ILidarDriver *lidar = *sl::createLidarDriver();
    if (!lidar) {
        std::cerr << "Insufficent memory, exit" << std::endl;
        exit(-1);
    }

    do {
        sl::IChannel *channel = *sl::createSerialPortChannel("/dev/ttyUSB0", 115200);

        if (SL_IS_FAIL(lidar->connect(channel)))
            std::cerr << "Error, cannot bind to the specified serial port /dev/ttyUSB0" << std::endl;

        sl_lidar_response_device_info_t devinfo;
        sl_result operat_result = lidar->getDeviceInfo(devinfo);

        if (SL_IS_FAIL(operat_result)) {
            if (operat_result == SL_RESULT_OPERATION_TIMEOUT)
                std::cerr << "Error, operation time out" << std::endl;
            else
                std::cerr << "Error, unexpected error, code: " << operat_result << std::endl;
        }

        std::cout << "Version: " << devinfo.hardware_version << std::endl;

        sl_lidar_response_device_health_t health_info;
        operat_result = lidar->getHealth(health_info);
        if (SL_IS_OK(operat_result)) {
            std::cout << "Lidar health status : " << std::endl;
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
            std::cerr << "errorcode: " << health_info.error_code;

        } else {
            std::cerr << "Error, cannot retrieve the lidar health code: " << operat_result << std::endl;
            break;
        }

        if (health_info.status == SL_LIDAR_STATUS_ERROR) {
            std::cerr << "Error, slamtec lidar internal error detected. Please reboot the device to retry" << std::endl;
            lidar->reset();
            break;
        }

        lidar->setMotorSpeed();

        if (SL_IS_FAIL(lidar->startScan(0, 1))) {
            std::cerr << "Error, cannot start the scan operation" << std::endl;
            break;
        }

        delay(3000);

        if (SL_IS_FAIL(get_point_info_one_cycle(lidar))) {
            std::cerr << "Error, cannot grab scan data" << std::endl;
            break;
        }
    } while (0);

    lidar->stop();
    delay(20);
    lidar->setMotorSpeed(0);

    if (lidar) {
        delete lidar;
        lidar = nullptr;
    }
    return 0;
}


