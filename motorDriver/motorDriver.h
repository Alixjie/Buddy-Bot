#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <pigpio.h>
#include <thread>
#include <mutex>

void AIN(int mode);
#endif