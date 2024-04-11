#include "pwm.hpp"
#include <unistd.h>
#include <iostream>
#include <math.h>
/*

REQUIRES
TO BUILD

g++ -o pwm motorDriver/pwm.cpp -lpigpio -lrt

TO RUN

sudo ./pwm
*/
//左轮方向
void AIN(int mode)
{
	int gpioPin1=6,gpioPin2=5;
//正转
	if(mode==1)
	{
    gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 0);
	gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 1);
	}
//反转
	 if(mode==0)   
	{
	gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 1);
	gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 0);
	 }
}

//右轮方向

void BIN(int mode)
{
	int gpioPin1=13,gpioPin2=26;
//正转
	if(mode==1)
	{
    gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 0);
	gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 1);
	}
//反转
	 if(mode==0)   
	{
	gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 1);
	gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 0);
	 }
}
void ClearBIN()
{
    int gpioPin1 = 13, gpioPin2 = 26;

    // 将 GPIO 引脚复位为低电平
    gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 0);
    gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 0);
}
void ClearAIN()
{
    int gpioPin1 = 6, gpioPin2 = 5;

    // 将 GPIO 引脚复位为低电平
    gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 0);
    gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 0);
}
void Set_Pwm() 
{
 //   if (v > 100) v = 100;
   gpioSetMode(22, PI_OUTPUT);
   gpioSetMode(27, PI_OUTPUT);
   gpioSetPWMfrequency(22, 10000);
   gpioSetPWMfrequency(27, 10000);
    
}
