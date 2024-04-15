#ifndef __ULM3_SAMPLES_H__
#define __ULM3_SAMPLES_H__

// #include "kalman_filter.h"
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "sync_queue.h"
#include "ulm3_acquisition_callback.h"
#include "ulm3_pdoa_comm.h"

typedef struct output_data {
    double x;
    double y;
} output_data;

class ULM3Samples {
public:
    ULM3Samples(const char* pname);
    // ULM3Samples();
    //~ULM3Samples();
    void start();
    output_data getData();
    control_param getControl();
    void controlCar();

    /*void registerControl(void (*distanceControl)(int),
                         void (*degreeControl)(int));*/
    // void startFollow();
    // void stopFollow();

private:
    bool isFirst_;
    // std::mutex mutex_;
    // int following_;
    // std::thread followThread;
    // KalmanFilter filter_;
    static constexpr char default_name[] = "/dev/ttyUSB1";
    SyncQueue<control_param> sync_queue_;
    ULM3PDOAComm ulm3_pdoa_comm_;
    ULM3AcquisitionCallback ulm3_acquisition_callback_;

    // void (*distanceControl_)(int);
    // void (*degreeControl_)(int);

    // void run_follow();
};

#endif
