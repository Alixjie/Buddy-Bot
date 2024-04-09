#include <cstddef>
#include <cstdio>
#include <iostream>

#include "ULM3_PDOAComm.h"

class ULM3PDOAprintSampleCallback : public ULM3PDOACallback {
    typedef struct uwb_data {
        int tag_id;
        int x_cm;
        int y_cm;
        int distance_cm;
        int range_num;
        float pdoa_deg;
        float aoa_deg;
        int distance_offset_cm;
        int pdoa_offset_deg;
    } uwb_data;

    typedef struct velocity {
        float speed;
        float degree;
    } velocity;

    // "MP0036,0,23,0,122,119,-156.33,-70.32,52,16"
    void pretreatment(const char *s, uwb_data &received_data)
    {
        int temp;
        sscanf(s, "MP%d,%d,%d,%d,%d,%d,%f,%f,%d,%d", &temp,
               &received_data.tag_id, &received_data.x_cm, &received_data.y_cm,
               &received_data.distance_cm, &received_data.range_num,
               &received_data.pdoa_deg, &received_data.aoa_deg,
               &received_data.distance_offset_cm,
               &received_data.pdoa_offset_deg);

        if (received_data.distance_cm < 0) {
            received_data.distance_cm = -received_data.distance_cm;
        }
    }

    velocity &control_velocity(const char *reveived_char)
    {
        uwb_data current_data;
        static velocity car_velocity = {10, 0};
        static int speed_step = 4;

        pretreatment(reveived_char, current_data);

        car_velocity.degree = current_data.aoa_deg;
        car_velocity.speed = 50;

        return car_velocity;
    }

    virtual void hasSample(char *uwb_chars, size_t length)
    {
        // velocity temp = control_velocity(uwb_chars);
        // std::cout << "Speed: " << temp.speed << " Degree: " << temp.degree
        //         << '\n';
        // printf("%s\n", uwb_chars);
        for (int i = 0; i < length; i++) {
            std::cout << uwb_chars[i];
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