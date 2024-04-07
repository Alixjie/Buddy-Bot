#include <stdio.h>

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
    sscanf(s, "MP%d,%d,%d,%d,%d,%d,%f,%f,%d,%d", &temp, &received_data.tag_id,
           &received_data.x_cm, &received_data.y_cm, &received_data.distance_cm,
           &received_data.range_num, &received_data.pdoa_deg,
           &received_data.aoa_deg, &received_data.distance_offset_cm,
           &received_data.pdoa_offset_deg);

    if (received_data.distance_cm < 0) {
        received_data.distance_cm = -received_data.distance_cm;
    }
}

velocity &control_velocity(const char *reveived_char)
{
    static uwb_data preview_data;
    static uwb_data current_data;
    static velocity car_velocity = {10, 0};
    static int speed_step = 4;

    preview_data = current_data;

    pretreatment(reveived_char, current_data);

    if (current_data.distance_cm > preview_data.distance_cm) {
        car_velocity.speed += speed_step;
    }
    car_velocity.degree = current_data.aoa_deg;

    return car_velocity;
}