#ifndef __ULM3_ACQUISTION_CALLBACK_H__
#define __ULM3_ACQUISTION_CALLBACK_H__

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "sync_queue.h"
#include "ulm3_pdoa_comm.h"

typedef struct control_param {
    int speed;
    double distance;
    double degree;
} control_param;

class ULM3AcquisitionCallback : public ULM3PDOACallback {
public:
    ULM3AcquisitionCallback(SyncQueue<control_param> *sync_queue);
    ~ULM3AcquisitionCallback();

    virtual void hasSample(const simple_string &uwb_string);

private:
    char *split_buffer_ = nullptr;
    uint_fast8_t sample_counter_;
    uwb_sample current_data_;
    SyncQueue<control_param> *sync_queue_;

    void getSample(const simple_string &sip_str);
    void monitorQueue();
};

#endif