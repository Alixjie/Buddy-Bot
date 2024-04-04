#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <unistd.h>
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
void Set_Pwm(int GPIO, int v) {
    if (v > 200) v = 200;

    gpioSetMode(GPIO, PI_OUTPUT);
    gpioSetPWMfrequency(GPIO, 10000);
    gpioPWM(GPIO, v);
}
void direction(int mode)
{
//前进
if(mode==0)
	{
    AIN(1);
    BIN(1);
	}
//后退
if(mode==1)   
	{
    AIN(0);
    BIN(0);
	 }
//左转
if(mode==2)
	{
    AIN(0);
    BIN(1);
	}
//右转
if(mode==3)   
	{
    AIN(1);
    BIN(0);
	 }
}
int main() {
    if (gpioInitialise() < 0) {
        fprintf(stderr, "Could not init pigpio.\n");
        return 1;
    }

    // 设置两个 GPIO 为指定的 PWM 值


    Set_Pwm(27, 50);
    Set_Pwm(22, 50);
    direction(0);
    getchar();
    direction(1);
    getchar();
    direction(2);
    getchar();
    direction(3);
    getchar();
    // 停止 PWM 输出
    gpioPWM(27, 0);
    gpioPWM(22, 0);
    ClearAIN();
    ClearBIN();
    // 清理
    gpioTerminate();

    return 0;
}