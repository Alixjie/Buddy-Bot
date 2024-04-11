#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <iostream>
#include <math.h>
#include <iostream>
#include <pigpio.h>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <unistd.h>
int VelocityR(int Target_RV, int Current_RV);
int VelocityL(int Target_LV, int Current_LV);
void Speed(int current_vL,int current_vR);
void spin(int angle);
void direction(int mode);
void clearALL();
#endif