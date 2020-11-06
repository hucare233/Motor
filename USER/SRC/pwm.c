#include "pwm.h"

/**
  *@brief 定时器2、3初始化函数 PA1--PA3对应TIM2_CH2--TIM2_CH4,PA6/PA7/PB0/PB1对应TIM3_CH1--TIM3_CH4
	**/
	
void PWM_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	GPIO_InitTypeDef GPIO_initStructre;	
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4 ,ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB ,ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
	GPIO_initStructre.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6|GPIO_Pin_7;
	
	GPIO_initStructre.GPIO_Mode = GPIO_Mode_AF;
	GPIO_initStructre.GPIO_OType = GPIO_OType_PP;
	GPIO_initStructre.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_initStructre.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_initStructre);	
	
//	GPIO_initStructre.GPIO_Mode=GPIO_Mode_OUT;
//	GPIO_initStructre.GPIO_OType=GPIO_OType_PP;
//	   
//	GPIO_initStructre.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_7 ;
//  GPIO_initStructre.GPIO_Speed=GPIO_Speed_2MHz;
//  GPIO_Init(GPIOB,&GPIO_initStructre);
//  
//	
//	
//	GPIOB->BSRRH = GPIO_Pin_3;
//	GPIOB->BSRRL = GPIO_Pin_4;
//	GPIOB->BSRRL = GPIO_Pin_7;
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;
	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	
	
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);										//PB3,初始化外设TIM2-CH2
	
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);										//PB4,初始化外设TIM3-CH1
		
	TIM_OC2Init(TIM4,&TIM_OCInitStructure);	
	
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);					
		
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);				
	
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM2,ENABLE);
	
	TIM_ARRPreloadConfig(TIM3,ENABLE);
  TIM_ARRPreloadConfig(TIM4,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	TIM_Cmd(TIM4,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
}
/**
  *@brief pwm波输出函数
	**/
void pwm_out(struct Engine_Datadef * engine)
{
	switch (engine->port)
	{					
			case PB3:
				
			if(engine->Start_Flag==1)
			{
				TIM_SetCompare2(TIM2,engine->Start_Ratio * temp.Engine_PWM_arr);
				
			  engine->Start_Flag=0;
			}
			else if(engine->Move_Flag == 1)
			{
				TIM_SetCompare2(TIM2,engine->Move_Ratio * temp.Engine_PWM_arr);
			}
			else if(engine->End_Flag == 1)
			{
				engine->End_Flag = 0;
				
				TIM_SetCompare2(TIM2,engine->End_Ratio * temp.Engine_PWM_arr);
			}
			break;
			
			case PB4:
				
			if(engine->Start_Flag==1)
			{
				TIM_SetCompare2(TIM2,engine->Start_Ratio * temp.Engine_PWM_arr);
				
			  engine->Start_Flag=0;
			}
			else if(engine->Move_Flag == 1)
			{
				TIM_SetCompare2(TIM2,engine->Move_Ratio * temp.Engine_PWM_arr);
			}
			else if(engine->End_Flag == 1)
			{
				engine->End_Flag = 0;
				
				TIM_SetCompare2(TIM2,engine->End_Ratio * temp.Engine_PWM_arr);
			}
			break;
						
			default: break;
		}
}

