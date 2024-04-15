#include <pigpio.h>
#include "motorControl/encoder.hpp"
#include "motorControl/pwm.hpp"
#include "motorControl/motorDriver.hpp"
#include "motorControl/movement.hpp"
#include "movecontrol/movecontrol.h"
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


static void timercallback(void){
   //  read mode, distance and angle
   mode = MoveControll.getInstance().getMovestate();
   int distance = MoveControll.getInstance().getDistance();
   int angle = MoveControll.getInstance().getAngle();

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

   if (mode == 0)
   {
   gpioPWM(27, 0);
   gpioPWM(22, 0); 
   target_count = 0;
   MoveControll.getInstance().sem.signal();// Set the sign of work completed
   }
   else if(mode == 1)//go straight
   {
      if(target_count > (18*distance)) // 10cm and longer:  target_count= 18*x(cm)
      { 
         gpioPWM(27, 0); gpioPWM(22, 0); 
         target_count = 0;
         MoveControll.getInstance().setMovestate(0);
      }
      Speed(vL_current, vR_current); 
      //printf("target_count = %d\n", target_count);

   }    
   else if(mode == 2) //spin
   {
      if(abs(target_count)>(abs(angle)*(10/3 +0.004*abs(angle))))   // target_count= angle*10/3 + 0.004 angle*angle
      {
         gpioPWM(27, 0);  gpioPWM(22, 0);
         target_count = 0;
          MoveControll.getInstance().setMovestate(0);
      } 
      spinSpeed(b,vL_current, vR_current);
   }
   
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
   //clear all
   gpioSetTimerFunc(0, 0, nullptr);
   clearALL();
   decL.re_cancel();
   decR.re_cancel();
   gpioTerminate();

}
