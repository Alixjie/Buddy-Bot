#include "motorDriver.hpp"
#include <cmath>
#include "encoder.hpp"
#include "pwm.hpp"
/*

一圈计数265
*/



void clearALL()
{
	gpioPWM(27, 0);
    gpioPWM(22, 0);
    ClearAIN();
    ClearBIN();
}

int VelocityL(int Target_LV, int Current_LV)
{
		int BiasL; 
		static int ControlVelocity, Last_bias;

		BiasL = Target_LV - Current_LV; 
		
	   ControlVelocity += 1.1*(BiasL-Last_bias) + 0.5*BiasL + 0.1;
      //ControlVelocity = Target_V;
		Last_bias=BiasL;	
		return ControlVelocity;
	
}

int VelocityR(int Target_RV, int Current_RV)
{
		int BiasR; 
		static int ControlVelocity, Last_bias;

		BiasR = Target_RV - Current_RV; 
		
	    ControlVelocity += 1.1*(BiasR-Last_bias) + 0.5*BiasR -0.3;
      //ControlVelocity = Target_V;
		Last_bias=BiasR;	
		return ControlVelocity;
	
}

void Speed(int current_vL,int current_vR)
{
    int vL, vR; 
    direction(0);
    vL = VelocityL(60,current_vL); 
    vR = VelocityR(60,current_vR); 
   // std::cout<<vL<<", "<<vR<<std::endl;
    gpioPWM(27,vR);
    gpioPWM(22,vL);
}
void spinSpeed(int b, int current_vL,int current_vR)
{
    int vL, vR; 
    
    if(b>0) //右转
    {
    AIN(1);
    ClearBIN();
    vL = VelocityL(50,abs(current_vL)); 
    vR = 0;
    }
    if(b<0) //左转
    {
    ClearAIN();
    BIN(1);
    vL = 0;
    vR = VelocityR(50,abs(current_vR)); 
    }
//    std::cout<<vL<<", "<<vR<<std::endl;
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
    ClearAIN();
    BIN(1);
	}
//右转
if(mode==3)   
	{
    AIN(1);
    ClearBIN();
	 }
//停止
if(mode==4)
   {
    gpioPWM(27, 0);
    gpioPWM(22, 0);
   }

}
/*
static void timercallback1()
{
    gpioPWM(27, 0);
    gpioPWM(22, 0);
    gpioSetTimerFunc(1,0,nullptr);
}
*/

