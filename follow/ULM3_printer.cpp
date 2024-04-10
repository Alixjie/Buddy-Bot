#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "ulm3_pdoa_comm.h"

class ULM3PDOAprintSampleCallback : public ULM3PDOACallback {
public:
    typedef struct location_data {
        int speed;
        double degree;
        double distance;
    } location_data;

    ULM3PDOAprintSampleCallback()
    {
        split_buffer_ = nullptr;
        sample_counter = 0;
    }

    ~ULM3PDOAprintSampleCallback()
    {
        if (split_buffer_ != nullptr) {
            delete[] split_buffer_;
        }
    }

    // "MP0036,0,23,0,122,119,-156.33,-70.32,52,16"

    /* velocity &control_velocity(const char *reveived_char)
    {
        uwb_sample current_data;
        static velocity car_velocity = {10, 0};
        static int speed_step = 4;

        pretreatment(reveived_char, current_data);

        car_velocity.degree = current_data.aoa_deg;
        car_velocity.speed = 50;

        return car_velocity;
    } */

    virtual void hasSample(const simple_string &uwb_string)
    {
        /*if (++sample_counter < 5) {
            return;
        }*/
        uwb_sample temp_sample;
        pretreatment(uwb_string, temp_sample);
        std::cout << " Distance: " << temp_sample.distance_cm
                  << " Degree: " << temp_sample.aoa_deg
                  << " Range: " << temp_sample.range_num << '\n';
    }

private:
    char *split_buffer_ = nullptr;
    uint_fast8_t sample_counter;

    void pretreatment(const simple_string &sip_str, uwb_sample &received_data)
    {
        if (split_buffer_ == nullptr) {
            split_buffer_ = new char[sip_str.capacity];
        }
        memcpy(split_buffer_, sip_str.str, sip_str.length);
        split_buffer_[sip_str.length] = '\0';

        int temp;
        sscanf(split_buffer_, "MP%d,%d,%d,%d,%d,%d,%lf,%lf,%d,%d", &temp,
               &received_data.tag_id, &received_data.x_cm, &received_data.y_cm,
               &received_data.distance_cm, &received_data.range_num,
               &received_data.pdoa_deg, &received_data.aoa_deg,
               &received_data.distance_offset_cm,
               &received_data.pdoa_offset_deg);

        if (received_data.distance_cm < 0) {
            received_data.distance_cm = -received_data.distance_cm;
        }
    }
};

int main(int argc, char *argv[])
{
    char pname[] = "/dev/ttyUSB0";
    ULM3PDOAComm ulm3_pdoa_comm(pname);
    ULM3PDOAprintSampleCallback ulm3_pdoa_print_sample_callback;
    ulm3_pdoa_comm.registerCallback(&ulm3_pdoa_print_sample_callback);
    ulm3_pdoa_comm.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    return 0;
}