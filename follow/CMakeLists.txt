cmake_minimum_required(VERSION 3.7.0)
project(car-follow VERSION 1.0 LANGUAGES "CXX")

set(CMAKE_CXX_STANDARD 17)

#add_executable(car_follow_test
#    ULM3_PDOAComm.h
#    ULM3_PDOAComm.cpp
#    ULM3_printer.cpp
#)

add_executable(callback_test
    sync_queue.h
    ulm3_acquisition_callback.h
    ulm3_acquisition_callback.cpp
    ulm3_pdoa_comm.h
    ulm3_pdoa_comm.cpp
    test_callback.cpp
)

#add_executable(filter_test
#    kalman_filter.cpp
#    kalman_filter.h
#    test_filter.cpp
#    )

#add_executable(finally_test
#    ulm3_samples.h
#    ulm3_samples.cpp
#    kalman_filter.h
#    kalman_filter.cpp
#    sync_queue.h
#    ulm3_acquisition_callback.h
#    ulm3_acquisition_callback.cpp
#    ulm3_pdoa_comm.h
#    ulm3_pdoa_comm.cpp)