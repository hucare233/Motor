#include "param.h"

Temp_Datadef temp;

struct Engine_Datadef Steering_Engine[4];

/**
  *@brief 变量初始化函数
	**/
void param_Init(void)
{	
	temp.Engine_PWM_arr =8999;
	
	temp.Engine_PWM_psc =159;
	
	
	Steering_Engine[0].port=PB3;
	
	Steering_Engine[0].Start_Flag=1;
	
	Steering_Engine[0].Start_Ratio=1;
	
	Steering_Engine[0].Move_Flag=0;
	
	Steering_Engine[0].Move_Ratio =1;
	
	Steering_Engine[0].End_Flag=0;
	
	Steering_Engine[0].End_Ratio=0;
	
	
	Steering_Engine[1].port=PB4;
	
	Steering_Engine[1].Start_Flag=1;
	
	Steering_Engine[1].Start_Ratio=0;
	
	Steering_Engine[1].Move_Flag=0;
	
	Steering_Engine[1].Move_Ratio =0;
	
	Steering_Engine[1].End_Flag=0;
	
	Steering_Engine[1].End_Ratio=0;
	
	
}

