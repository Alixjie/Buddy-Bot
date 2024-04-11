#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <ostream>
#include <thread>

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

int open_dev(char *Dev)
{
    int fd = 0;
    fd = open(Dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        perror("Error opening serial port.");
        return 1;
    }
    return fd;
}

int main()
{
    int fd;
    struct termios tty;
    char buffer[1024];
    ssize_t bytesRead;

    char pname[] = "/dev/ttyUSB0";
    fd = open_dev(pname);

    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "Error getting serial port attributes." << std::endl;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag |= PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(INPCK | ISTRIP);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "Error setting serial port attributes." << std::endl;
        close(fd);
        return 1;
    }

    // Read data from serial port
    while (true) {
        bytesRead = read(fd, buffer, sizeof(buffer));
        /* buffer[bytesRead + 1] = '\0';
        printf("fd:%d, nread:%ld, buff:%s\n", fd, bytesRead, buffer); */
        /* if (bytesRead == -1) {
            std::cerr << "Error reding serial port." << std::endl;
            close(fd);
            return 1;
        } */
        // if (bytesRead > 0) {
        for (int i = 0; i < bytesRead; i++) {
            std::cout << buffer[i];
        }
        //}

        //velocity tmp = control_velocity(buffer);
        //std::cout << tmp.degree << ' ' << tmp.speed << '\n';

        // printf("%s\n", buffer);

        // std::this_thread::sleep_for(std::chrono::milliseconds(110));
    }
    close(fd);
    return 0;
}
