QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AStar.cpp \
    VoiceControl/audio/audio_buffer.c \
    VoiceControl/audio/pa_list_devices.c \
    VoiceControl/audio/wav_io.c \
    VoiceControl/stt.cpp \
    arch/linux/net_serial.cpp \
    arch/linux/net_socket.cpp \
    arch/linux/timer.cpp \
    dataunpacker/dataunpacker.cpp \
    dataunpacker/unpacker/handler_capsules.cpp \
    dataunpacker/unpacker/handler_hqnode.cpp \
    dataunpacker/unpacker/handler_normalnode.cpp \
    follow/kalman_filter.cpp \
    follow/ulm3_acquisition_callback.cpp \
    follow/ulm3_pdoa_comm.cpp \
    follow/ulm3_samples.cpp \
    hal/thread.cpp \
    main.cpp \
    mainwindow.cpp \
    motorControl/encoder.cpp \
    motorControl/motorDriver.cpp \
    motorControl/movement.cpp \
    motorControl/pwm.cpp \
    movecontrol/movecontrol.cpp \
    rplidar_driver.cpp \
    sl_async_transceiver.cpp \
    sl_crc.cpp \
    sl_lidar_driver.cpp \
    sl_lidarprotocol_codec.cpp \
    sl_serial_channel.cpp \
    sl_tcp_channel.cpp \
    sl_udp_channel.cpp \
    utils/semaphore.cpp

HEADERS += \
    AStar.h \
    VoiceControl/audio/audio_buffer.h \
    VoiceControl/audio/pa_list_devices.h \
    VoiceControl/audio/wav_io.h \
    VoiceControl/coqui-stt.h \
    VoiceControl/stt.h \
    arch/linux/arch_linux.h \
    arch/linux/net_serial.h \
    arch/linux/thread.hpp \
    arch/linux/timer.h \
    dataunpacker/dataunnpacker_commondef.h \
    dataunpacker/dataunnpacker_internal.h \
    dataunpacker/dataunpacker.h \
    dataunpacker/dataupacker_namespace.h \
    dataunpacker/unpacker/handler_capsules.h \
    dataunpacker/unpacker/handler_hqnode.h \
    dataunpacker/unpacker/handler_normalnode.h \
    follow/kalman_filter.h \
    follow/sync_queue.h \
    follow/ulm3_acquisition_callback.h \
    follow/ulm3_pdoa_comm.h \
    follow/ulm3_samples.h \
    hal/abs_rxtx.h \
    hal/assert.h \
    hal/byteops.h \
    hal/byteorder.h \
    hal/event.h \
    hal/locker.h \
    hal/socket.h \
    hal/thread.h \
    hal/types.h \
    hal/util.h \
    hal/waiter.h \
    mainwindow.h \
    motorControl/encoder.hpp \
    motorControl/motorDriver.hpp \
    motorControl/movement.hpp \
    motorControl/pwm.hpp \
    movecontrol/movecontrol.h \
    rplidar.h \
    rplidar_cmd.h \
    rplidar_driver.h \
    rplidar_protocol.h \
    rptypes.h \
    sdkcommon.h \
    sl_async_transceiver.h \
    sl_crc.h \
    sl_lidar.h \
    sl_lidar_cmd.h \
    sl_lidar_driver.h \
    sl_lidar_driver_impl.h \
    sl_lidar_protocol.h \
    sl_lidarprotocol_codec.h \
    sl_types.h \
    utils/semaphore.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += $$PWD/VoiceControl/lib/libkenlm.so
LIBS += $$PWD/VoiceControl/lib/libstt.so

unix:!macx: LIBS += -L$$PWD/VoicceControl/lib/ -lportaudio -lpulse-simple -lpulse -lstt

INCLUDEPATH += $$PWD/VoiceControl/lib
DEPENDPATH += $$PWD/VoiceControl/lib
