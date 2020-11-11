/*
 * @Descripttion: 定时器2超时检测
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-09 22:10:47
 * @FilePath: \MotoPro\USER\SRC\tim2.c
 */
#include "tim2.h"

void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitStructure.TIM_Period = 6999; //2ms
	TIM_TimeBaseInitStructure.TIM_Prescaler =5;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) //溢出中断
	{
#ifdef USE_VESC
		for (u8 i = 0; i < 4; i++)
		{
			/* 反馈超时判断 */
			if ((VESCmotor[i].argum.timeout == 1)&&VESCmotor[i].enable && ((OSTimeGet() - VESCmotor[i].argum.lastRxTim) > VESCmotor[i].argum.timeoutTicks&&(VESCmotor[i].argum.timeout == 1)))
				VESCmotor[i].argum.timeoutCnt++; //反馈超时判断
			else
				VESCmotor[i].argum.timeoutCnt = 0;
			if (VESCmotor[i].argum.timeoutCnt > 100)
			{
				VESCmotor[i].status.timeout = true;
				Beep_Show(2);
				insertError(Eerror.head, VESCERROR | ((i + 1) << 4) | TIMEOUT);
				Led8DisData(2);
			}
			else
			{
				Led8DisData(0);
				VESCmotor[i].status.timeout = false;
				deleteError(Eerror.head, ErrorFind(Eerror.head, VESCERROR | ((i + 1) << 4) | TIMEOUT));
			}
		}
#endif
#ifdef USE_ELMO
		for (u32 i = 0; i < 4; i++) //ELMO超时判断
		{
			if (ELMOmotor[i].enable)
			{
				/* 反馈超时判断 */
				if ((OSTimeGet() - ELMOmotor[i].argum.lastRxTim) > ELMOmotor[i].argum.timeoutTicks && (ELMOmotor[i].argum.timeout == 1))
					ELMOmotor[i].argum.timeoutCnt++; //反馈超时判断
				else
					ELMOmotor[i].argum.timeoutCnt = 0;
				if (ELMOmotor[i].argum.timeoutCnt > 1000)
				{
					ELMOmotor[i].status.timeout = true;
					Beep_Show(2);
					Led8DisData(3);
					Delay_ms(500);
				}
				else
				{
					Led8DisData(0);
					ELMOmotor[i].status.timeout = false;
				}
			}
		}
#endif
#ifdef USE_DJ
		for (u8 i = 0; i < 8; i++)
			iftimeout(i); //DJ超时检测
#endif
#ifdef USE_EPOS
		for (u8 i = 0; i < 4; i++) //EPOS超时判断
		{
			if (EPOSmotor[i].enable)
			{
				/* 反馈超时判断 */
				if ((OSTimeGet() - EPOSmotor[i].argum.lastRxTim) > EPOSmotor[i].argum.timeoutTicks && (EPOSmotor[i].argum.timeout == 1))
					EPOSmotor[i].argum.timeoutCnt++; //反馈超时判断
				else
					EPOSmotor[i].argum.timeoutCnt = 0;
				if (EPOSmotor[i].argum.timeoutCnt > 100)
				{
					EPOSmotor[i].status.timeout = true;
					Beep_Show(2);
					Led8DisData(4);
					Delay_ms(200);
				}
				else
				{
					Led8DisData(0);
					EPOSmotor[i].status.timeout = false;
				}
			}
		}
#endif
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断标志位
}
