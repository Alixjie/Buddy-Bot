QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    follow/kalman_filter.cpp \
    follow/ulm3_acquisition_callback.cpp \
    follow/ulm3_pdoa_comm.cpp \
    follow/ulm3_samples.cpp \
    main.cpp \
    mainwindow.cpp \
    motorControl/encoder.cpp \
    motorControl/motorDriver.cpp \
    motorControl/movement.cpp \
    motorControl/pwm.cpp \
    movecontrol/movecontrol.cpp \
    rplidar/AStar.cpp \
    rplidar/arch/linux/net_serial.cpp \
    rplidar/arch/linux/net_socket.cpp \
    rplidar/arch/linux/timer.cpp \
    rplidar/dataunpacker/dataunpacker.cpp \
    rplidar/dataunpacker/unpacker/handler_capsules.cpp \
    rplidar/dataunpacker/unpacker/handler_hqnode.cpp \
    rplidar/dataunpacker/unpacker/handler_normalnode.cpp \
    rplidar/hal/thread.cpp \
    rplidar/rplidar_driver.cpp \
    rplidar/sl_async_transceiver.cpp \
    rplidar/sl_crc.cpp \
    rplidar/sl_lidar_driver.cpp \
    rplidar/sl_lidarprotocol_codec.cpp \
    rplidar/sl_serial_channel.cpp \
    rplidar/sl_tcp_channel.cpp \
    rplidar/sl_udp_channel.cpp \
    utils/semaphore.cpp \
    voicecontrol/voicecontrol.cpp

HEADERS += \
    follow/kalman_filter.h \
    follow/sync_queue.h \
    follow/ulm3_acquisition_callback.h \
    follow/ulm3_pdoa_comm.h \
    follow/ulm3_samples.h \
    mainwindow.h \
    motorControl/encoder.hpp \
    motorControl/motorDriver.hpp \
    motorControl/movement.hpp \
    motorControl/pwm.hpp \
    movecontrol/movecontrol.h \
    rplidar/AStar.h \
    rplidar/arch/linux/arch_linux.h \
    rplidar/arch/linux/net_serial.h \
    rplidar/arch/linux/thread.hpp \
    rplidar/arch/linux/timer.h \
    rplidar/dataunpacker/dataunnpacker_commondef.h \
    rplidar/dataunpacker/dataunnpacker_internal.h \
    rplidar/dataunpacker/dataunpacker.h \
    rplidar/dataunpacker/dataupacker_namespace.h \
    rplidar/dataunpacker/unpacker/handler_capsules.h \
    rplidar/dataunpacker/unpacker/handler_hqnode.h \
    rplidar/dataunpacker/unpacker/handler_normalnode.h \
    rplidar/hal/abs_rxtx.h \
    rplidar/hal/assert.h \
    rplidar/hal/byteops.h \
    rplidar/hal/byteorder.h \
    rplidar/hal/event.h \
    rplidar/hal/locker.h \
    rplidar/hal/socket.h \
    rplidar/hal/thread.h \
    rplidar/hal/types.h \
    rplidar/hal/util.h \
    rplidar/hal/waiter.h \
    rplidar/rplidar.h \
    rplidar/rplidar_cmd.h \
    rplidar/rplidar_driver.h \
    rplidar/rplidar_protocol.h \
    rplidar/rptypes.h \
    rplidar/sdkcommon.h \
    rplidar/sl_async_transceiver.h \
    rplidar/sl_crc.h \
    rplidar/sl_lidar.h \
    rplidar/sl_lidar_cmd.h \
    rplidar/sl_lidar_driver.h \
    rplidar/sl_lidar_driver_impl.h \
    rplidar/sl_lidar_protocol.h \
    rplidar/sl_lidarprotocol_codec.h \
    rplidar/sl_types.h \
    utils/semaphore.h \
    voicecontrol/voicecontrol.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -lpigpio -lrt

