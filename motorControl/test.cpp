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
volatile int countL=0, countR=0 ;
static int vL_current=0,vR_current=0,target_count=0, a = 1; 
static int b = 0;

void callbackL(int wayL)
{
   countL += wayL;
//   std::cout <<"COUNTL= "<< countL<< std::endl;
}
void callbackR(int wayR)
{
   countR += wayR;
  // std::cout <<"COUNTR= "<< countR<< std::endl;
}


static void timercallback(void){
   //  read a,b
   if((a==2)&&(b<0))//turn left
      {if(b<0) target_count += countR;}
   else//other condition
      {target_count += countL;}

   if(countR>0)
   vR_current = countR+30;
   if(countR<0)
   vR_current = countR-30;
   if(countL>0)
   vL_current = countL+30;
   if(countL<0)
   vL_current = countL-30;

   if (a == 0)
   {
   gpioPWM(27, 0);
   gpioPWM(22, 0); 
   }
   else if(a == 1)//go straight
   {
      Speed(vL_current, vR_current);
      printf("target_count = %d\n", target_count);
      else if(target_count > b) { gpioPWM(27, 0); gpioPWM(22, 0); target_count = 0;a = 0;}// 10cm and longer:  b = 18*x(cm)
   }    
   else if(a == 2) //spin
   {
      spinSpeed(b,vL_current, vR_current);
      if(abs(target_count)>b)   {gpioPWM(27, 0);  gpioPWM(22, 0);target_count = 0;;a = 0;} //90°  b = angle*10/3 + 0.004 angle*angle
   }

   printf("vL_current=%d vR_current=%d\n", vL_current, vR_current);

   countL = countR = 0;


}

int main(int argc, char *argv[])
{
   if (gpioInitialise() < 0) return 1;
   Set_Pwm(); 
   re_decoder decR(16, 12, callbackR);
   re_decoder decL(20, 21, callbackL);

   //Periodically calling timercallback to change the motion status and keep speed-loop.
   gpioSetTimerFunc(0, 54, timercallback); 
   
   //Control instruction
    a=2; b=-90;
   getchar();
    a=2;b=-45;
   getchar();
    a=2;b=-30;
   getchar();
    a=2;b=-15;    

   //clear all
   getchar();
   gpioSetTimerFunc(0, 0, nullptr);
   clearALL();
   decL.re_cancel();
   decR.re_cancel();
   gpioTerminate();

}

