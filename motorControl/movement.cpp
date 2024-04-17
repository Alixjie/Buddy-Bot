#include <pigpio.h>
#include "motorControl/encoder.hpp"
#include "motorControl/pwm.hpp"
#include "motorControl/motorDriver.hpp"
#include "motorControl/movement.hpp"
#include "movecontrol/movecontrol.h"

volatile int countL=0, countR=0;
static int vL_current=0,vR_current=0,target_count=0;
static int mode = 0,number = 0;
/*

TO BUILD


g++ -o rot_enc_c motorDriver/test.cpp motorDriver/encoder.cpp motorDriver/pwm.cpp motorDriver/motorDriver.cpp -lpigpio -lrt

TO RUN

sudo ./rot_enc_c

一圈计数265
*/
void callbackL(int wayL)
{
   countL += wayL;
}
void callbackR(int wayR)
{
   countR += wayR;
}


void timercallback(void){
   MoveControll& mc = MoveControll::getInstance();
   //  read mode, distance and angle
   mode = mc.getMovestate();
   int distance = mc.getDistance();
   int angle = mc.getAngle();


   if((mode==2)&&(angle<0))//turn left
      {target_count += countR;}
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


   //printf("mode=%d, distance=%d, angle=%d, countR=%d, countL=%d, target_count=%d\n", mode,distance,angle,countR,countL,target_count);
   if (mode == 0)
   {
   gpioPWM(27, 0);
   gpioPWM(22, 0); 
   if(countL == 0 && countR == 0){
       target_count = 0;
     // if(number==2){
         mc.sem.signal();// Set the sign of work completed
       //  number=0;
      //}
      //number+=1;
   }
   }
   else if(mode == 1)//go straight
   {
      if(target_count >= (18*distance)) // 10cm and longer:  target_count= 18*x(cm)
      { 
         gpioPWM(27, 0); gpioPWM(22, 0); 
         target_count = 0;
         mc.setMovestate(0);
      }
      Speed(vL_current, vR_current);
      //printf("target_count = %d\n", target_count);

   }    
   else if(mode == 2) //spin
   {
      if(abs(target_count)>=(abs(angle)*(10/3 +0.004*abs(angle))))   // target_count= angle*10/3 + 0.004 angle*angle
      {
         gpioPWM(27, 0);  gpioPWM(22, 0);
         target_count = 0;
         mc.setMovestate(0);
      } 
      spinSpeed(angle,vL_current, vR_current);
   }
   
   countL = countR = 0;
}
