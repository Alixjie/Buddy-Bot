#include "motorDriver.h"
#include <math.h>

extern float Velcity_Kp,  Velcity_Ki,  Velcity_Kd; 


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
