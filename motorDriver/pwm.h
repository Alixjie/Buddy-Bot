#ifndef __PWM_H
#define __PWM_H
#include <sys.h>	 
#include <pigpio.h> 
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>

void Set_Pwm(int moto1,int moto2);
void Set_PwmA(int PWM);
void Set_PwmB(int PWM);
#endif
 