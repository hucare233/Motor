#include "tim4.h"

/**
  *@brief 定时器4初始化函数
	**/
void TIM4_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
		
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
	
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
	TIM_Cmd(TIM4, ENABLE);
}

/**
  *@brief 定时器4中断服务函数
	**/

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
		
		pwm_out(&Steering_Engine[0]);
		
		
		//pwm_out(&Steering_Engine[1]);
		
	//	pwm_out(&Steering_Engine[2]);
		
	//	pwm_out(&Steering_Engine[3]);
	}
}

