#include <stdio.h>
#include <iostream>
#include <pigpio.h>
#include "encoder.hpp"
#include "pwm.hpp"
#include <unistd.h>
#include <math.h>
#include "motorDriver.hpp"
/*

TO BUILD


g++ -o rot_enc_c motorDriver/test.cpp motorDriver/encoder.cpp motorDriver/pwm.cpp motorDriver/motorDriver.cpp -lpigpio -lrt

TO RUN

sudo ./rot_enc_c
一圈计数265
*/
volatile int countL=0, countR=0;
static int vL_current=0,vR_current=0;

void callbackL(int wayL)
{
   countL += wayL;
//   std::cout <<"COUNTL= "<< countL<< std::endl;
}
void callbackR(int wayR)
{
   countR += wayR;
//  std::cout <<"COUNTR= "<< countR<< std::endl;
}


static void timercallback(void){

   vR_current = countR+30;
   vL_current = countL+30;

   printf("vL_current=%d vR_current=%d\n", vL_current, vR_current);

   //std::cout <<"vL_current= "<< vL_current<< std::endl;
   //std::cout <<"vR_current= "<< vR_current<< std::endl;
   countL = countR = 0;
}
int main(int argc, char *argv[])
{
   if (gpioInitialise() < 0) return 1;
   Set_Pwm(); 

   re_decoder decR(16, 12, callbackR);
   re_decoder decL(20, 21, callbackL);
   gpioSetTimerFunc(0, 54, timercallback); 
   Speed(vL_current,vR_current);

   //控制指令
   direction(0); 
   getchar();  
   //清空
   clearALL();
   decL.re_cancel();
   decR.re_cancel();
   gpioTerminate();

}

