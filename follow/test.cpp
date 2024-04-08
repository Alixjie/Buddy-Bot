#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <thread>

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
    tty.c_cflag &= ~PARENB;
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
        if (bytesRead == -1) {
            std::cerr << "Error reding serial prot." << std::endl;
            close(fd);
            return 1;
        }
        if (bytesRead > 0) {
            for (int i = 0; i < bytesRead; i++) {
                std::cout << buffer[i];
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    close(fd);
    return 0;
}