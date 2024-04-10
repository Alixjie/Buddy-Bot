#ifndef __ULM3_ACQUISTION_CALLBACK_H__
#define __ULM3_ACQUISTION_CALLBACK_H__

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "ulm3_pdoa_comm.h"

class ULM3AcquisitionCallback : public ULM3PDOACallback {
public:
    ULM3AcquisitionCallback();
    ~ULM3AcquisitionCallback();

    virtual void hasSample(const simple_string &uwb_string);

private:
    char *split_buffer_ = nullptr;
    uint_fast8_t sample_counter;
    uwb_sample current_data;

    void getSample(const simple_string &sip_str);
};

#endif