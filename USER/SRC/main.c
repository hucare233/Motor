#include "main.h"
s32 pwm;
int main(void)

{
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	param_Init();//设置各种参数的值
	Beep_Configuration();
	//TIM4_Init(8999,8);//TIM4
	PWM_Init(temp.Engine_PWM_arr ,temp.Engine_PWM_psc);//TIM2、TIM3
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	CAN_ITConfig(CAN1,CAN_IT_FMP1,ENABLE);
  u32 led0pwmval=0;
	u8 dir=1;
//	Beep_Show(2);
	//Sound(2);
	//play_Music_1();
	pwm=600;
	while(1)
	{
               Delay_ms(10);
//              if(dir)led0pwmval++;//dir==1 led0pwmval 递增
//               else led0pwmval--; //dir==0 led0pwmval 递减
//               if(led0pwmval>8000)dir=0;//led0pwmval 到达 300 后，方向为递减
//                   if(led0pwmval==0)dir=1; //led0pwmval 递减到 0 后，方向改为递增
                TIM_SetCompare2(TIM2,pwm); //修改比较值，修改占空比
		            TIM_SetCompare1(TIM3,4400);
		            TIM_SetCompare2(TIM4,4400);
	}            
}


