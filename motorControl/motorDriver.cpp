#include "motorDriver.hpp"
#include <math.h>
#include "encoder.hpp"
#include "pwm.hpp"
/*

一圈计数265
*/

static void timercallback1()
{
    gpioPWM(27, 0);
    gpioPWM(22, 0);
    ClearAIN();
    ClearBIN();
}
void spin(int angle)
{
   int t;
   t/54 = 265  // 每us计数60/54 转360°计数265 
   if(angle)
   direction(3); 
   else direction(2);
   gpioSetTimerFunc(1, t, timercallback1);  
}
void clearALL()
{
	gpioPWM(27, 0);
    gpioPWM(22, 0);
    ClearAIN();
    ClearBIN();
}
float Kp = 0,  Ki = 1,  Kd =0; 

int VelocityL(int Target_LV, int Current_LV)
{
		int BiasL; 
		static int ControlVelocity, Last_bias;

		BiasL = Target_LV - Current_LV; 
		
	   ControlVelocity += 0.1*(BiasL-Last_bias) + 0.9*BiasL + 1.8;
      //ControlVelocity = Target_V;
		Last_bias=BiasL;	
		return ControlVelocity;
	
}

int VelocityR(int Target_RV, int Current_RV)
{
		int BiasR; 
		static int ControlVelocity, Last_bias;

		BiasR = Target_RV - Current_RV; 
		
	   ControlVelocity += Kp*(BiasR-Last_bias) + Ki*BiasR + Kd;
      //ControlVelocity = Target_V;
		Last_bias=BiasR;	
		return ControlVelocity;
	
}

void Speed(int current_vL,int current_vR)
{
    static int vL, vR; 
    vL = VelocityL(60,current_vL); 
    vR = VelocityR(60,current_vR); 
    gpioPWM(27,vR);
    gpioPWM(22,vL);
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
//停止
if(mode==4)
   {
	gpioPWM(27, 0);
    gpioPWM(22, 0);
   }

}
