/*
 * @Descripttion: 定时器2超时检测
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-05 17:05:52
 * @FilePath: \MotoPro\USER\SRC\tim2.c
 */
#include "tim2.h"

void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitStructure.TIM_Period = 699999; //50ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 5;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) //溢出中断
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断标志位
#ifdef USE_VESC
		for (u8 i = 0; i < 4; i++)
		{
			/* 反馈超时判断 */
			if ((VESCmotor[i].argum.timeout == 1) && VESCmotor[i].enable && ((OSTimeGet() - VESCmotor[i].argum.lastRxTim) > VESCmotor[i].argum.timeoutTicks))
				VESCmotor[i].argum.timeoutCnt++; //反馈超时判断
			else
				VESCmotor[i].argum.timeoutCnt = 0;
			if (VESCmotor[i].argum.timeoutCnt > 20)
			{
				VESCmotor[i].status.timeout = true;
		    flag.MotorerrorFlag[3]=true;  
//				BEEP_ON;
//				OSTimeDly(1000);
//				BEEP_OFF;
//				OSTimeDly(1000);
				insertError(Eerror.head, VESCERROR | ((i + 1) << 4) | TIMEOUT);
				Led8DisData(2);
				Delay_ms(200);
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
		for (int i = 0; i < 4; i++) //ELMO超时判断
		{
			if (ELMOmotor[i].enable)
			{
				if (ELMOmotor[i].argum.timeout)
					ELMOmotor[i].argum.timecut++;
				else
					ELMOmotor[i].argum.timecut = 0;
				/* 反馈超时判断 */
				if ((OSTimeGet() - ELMOmotor[i].argum.lastRxTim) > ELMOmotor[i].argum.timeoutTicks)
					ELMOmotor[i].argum.timeoutCnt++; //反馈超时判断
				else
					ELMOmotor[i].argum.timeoutCnt = 0;
				if ((ELMOmotor[i].argum.timeoutCnt > 4) || (ELMOmotor[i].argum.timecut > 5))
				{
					ELMOmotor[i].status.timeout = true;
					flag.MotorerrorFlag[1]=true;
//					BEEP_ON;
//					OSTimeDly(1000);
//					BEEP_OFF;
//					OSTimeDly(1000);
					Led8DisData(3);
					OSTimeDly(100);
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
		for (int i = 0; i < 8; i++)
		{
			iftimeout(i); //DJ超时检测
		}
		if ((motor[0].status.timeout == 0) && (motor[1].status.timeout == 0) &&
			(motor[2].status.timeout == 0) && (motor[3].status.timeout == 0) &&
			(motor[4].status.timeout == 0) && (motor[5].status.timeout == 0) &&
			(motor[6].status.timeout == 0) && (motor[7].status.timeout == 0))
			sprintf(Motor_error, "%s", "(*_ *)");
#endif
#ifdef USE_EPOS
		for (int i = 0; i < 4; i++) //EPOS超时判断
		{
			if (EPOSmotor[i].enable)
			{
				if (EPOSmotor[i].argum.timeout)
					EPOSmotor[i].argum.timecut++;
				else
					EPOSmotor[i].argum.timecut = 0;
				/* 反馈超时判断 */
				if ((OSTimeGet() - EPOSmotor[i].argum.lastRxTim) > EPOSmotor[i].argum.timeoutTicks )
					EPOSmotor[i].argum.timeoutCnt++; //反馈超时判断
				else
					EPOSmotor[i].argum.timeoutCnt = 0;
				if (EPOSmotor[i].argum.timeoutCnt > 4 || (EPOSmotor[i].argum.timecut > 1))
				{
					EPOSmotor[i].status.timeout = true;
          flag.MotorerrorFlag[2]=true;
//					BEEP_ON;
//					OSTimeDly(1000);
//					BEEP_OFF;
//					OSTimeDly(1000);
					Led8DisData(4);
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
}
