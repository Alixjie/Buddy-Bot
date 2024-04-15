#ifndef MOVEMENT_H
#define MOVEMENT_H
#include <stdio.h>
#include <iostream>
#include <pigpio.h>
#include <unistd.h>
#include <math.h>
volatile int countL=0, countR=0 ;
static int vL_current=0,vR_current=0,target_count=0;
static int mode = 0;
//a = 1, b = 0; 

static void timercallback(void)
void callbackL(int wayL);
void callbackR(int wayR);

#endif