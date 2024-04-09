#include "ULM3_AcquisitionCallback.h"
#include "ULM3_PDOAComm.h"

ULM3AcquisitionCallback::ULM3AcquisitionCallback()
{
    split_buffer_ = nullptr;
    sample_counter = 0;
}

ULM3AcquisitionCallback::~ULM3AcquisitionCallback()
{
    if (split_buffer_!=nullptr){
        delete [] split_buffer_;
    }
}

void ULM3AcquisitionCallback::getSample(const simple_string &sip_str){
    if (split_buffer_ == nullptr) {
            split_buffer_ = new char[sip_str.capacity];
        }
        memcpy(split_buffer_, sip_str.str, sip_str.length);
        split_buffer_[sip_str.length] = '\0';

        int temp;
        sscanf(split_buffer_, "MP%d,%d,%d,%d,%d,%d,%lf,%lf,%d,%d", &temp,
               &current_data.tag_id, &current_data.x_cm, &current_data.y_cm,
               &current_data.distance_cm, &current_data.range_num,
               &current_data.pdoa_deg, &current_data.aoa_deg,
               &current_data.distance_offset_cm,
               &current_data.pdoa_offset_deg);
}