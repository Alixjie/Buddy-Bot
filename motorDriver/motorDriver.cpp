#include "motorDriver.h"
#include <math.h>

extern float Velcity_Kp,  Velcity_Ki,  Velcity_Kd; 


void AIN(int mode)
{
	int gpioPin1,gpioPin2;
    if (gpioInitialise() < 0) {
        return;
    }

	if(mode==1)
	{
    gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 0);
	gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 1);
	}
	 if(mode==0)   
	{
	gpioSetMode(gpioPin1, PI_OUTPUT);
    gpioWrite(gpioPin1, 1);
	gpioSetMode(gpioPin2, PI_OUTPUT);
    gpioWrite(gpioPin2, 0);
	 }
     usleep(3000000);
}

int Velocity_A(int TargetVelocity, int CurrentVelocity)
{
		int Bias; 
		static int ControlVelocity, Last_bias;
		
		Bias=TargetVelocity-CurrentVelocity; 
		
		ControlVelocity+=Velcity_Kp*(Bias-Last_bias)+Velcity_Ki*Bias;
		Last_bias=Bias;	
		return ControlVelocity;
	
}
int Velocity_B(int TargetVelocity, int CurrentVelocity)
{
		int Bias; 
		static int ControlVelocity, Last_bias;
		
		Bias=TargetVelocity-CurrentVelocity; 
		
		ControlVelocity+=Velcity_Kp*(Bias-Last_bias)+Velcity_Ki*Bias;
		Last_bias=Bias;	
		return ControlVelocity;
	
}
