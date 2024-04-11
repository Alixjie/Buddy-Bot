#ifndef __PWM_H
#define __PWM_H
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void AIN(int mode);
//右轮方向
void BIN(int mode);
void ClearBIN();
void ClearAIN();
void Set_Pwm();
#endif
 