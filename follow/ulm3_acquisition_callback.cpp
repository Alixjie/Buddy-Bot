#include "ulm3_acquisition_callback.h"

ULM3AcquisitionCallback::ULM3AcquisitionCallback(
    SyncQueue<control_param> *sync_queue)
{
    split_buffer_ = nullptr;
    sample_counter_ = 0;
    sync_queue_ = sync_queue;
}

ULM3AcquisitionCallback::~ULM3AcquisitionCallback()
{
    if (split_buffer_ != nullptr) {
        delete[] split_buffer_;
    }
}

void ULM3AcquisitionCallback::getSample(const simple_string &sip_str)
{
    if (split_buffer_ == nullptr) {
        split_buffer_ = new char[sip_str.capacity];
    }
    memcpy(split_buffer_, sip_str.str, sip_str.length);
    split_buffer_[sip_str.length] = '\0';

    int temp;
    sscanf(split_buffer_, "MP%d,%d,%d,%d,%d,%d,%lf,%lf,%d,%d", &temp,
           &current_data_.tag_id, &current_data_.x_cm, &current_data_.y_cm,
           &current_data_.distance_cm, &current_data_.range_num,
           &current_data_.pdoa_deg, &current_data_.aoa_deg,
           &current_data_.distance_offset_cm, &current_data_.pdoa_offset_deg);
}

void ULM3AcquisitionCallback::hasSample(const simple_string &uwb_string)
{
    getSample(uwb_string);
    control_param param;
    if (current_data_.distance_cm <= 40) {
        param.speed = 0;
    } else {
        param.speed = 1;
    }
    param.distance = current_data_.distance_cm;
    param.degree = current_data_.aoa_deg;

    struct timespec begin, end;

    clock_gettime(CLOCK_REALTIME, &begin);

    sync_queue_->push(param, 500);

    clock_gettime(CLOCK_REALTIME, &end);

    // Discard those data that has exsited in queue more than 500ms
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    long elapsed = seconds * 1000 + nanoseconds / 1000;

    if (elapsed > 500) {
        sync_queue_->clear();
    }
}
