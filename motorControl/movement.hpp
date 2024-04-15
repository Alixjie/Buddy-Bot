#ifndef MOVEMENT_H
#define MOVEMENT_H
#include <stdio.h>
#include <iostream>
#include <pigpio.h>
#include <unistd.h>
#include <math.h>

//a = 1, b = 0; 

static void timercallback(void);
void callbackL(int wayL);
void callbackR(int wayR);

#endif
