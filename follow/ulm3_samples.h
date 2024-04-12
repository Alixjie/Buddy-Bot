#ifndef __ULM3_SAMPLES_H__
#define __ULM3_SAMPLES_H__

#include "kalman_filter.h"
#include "sync_queue.h"
#include "ulm3_acquisition_callback.h"
#include "ulm3_pdoa_comm.h"

typedef struct output_data {
    double x;
    double y;
} output_data;

class ULM3Samples {
public:
    ULM3Samples(char* pname);
    void start();
    output_data getData();

private:
    bool isFirst_;
    KalmanFilter filter_;
    SyncQueue<control_param> sync_queue_;
    ULM3PDOAComm ulm3_pdoa_comm_;
    ULM3AcquisitionCallback ulm3_acquisition_callback_;
};

#endif
