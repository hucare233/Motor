/*
 * @Descripttion: 定时器三，报文发送
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-06 20:07:17
 * @FilePath: \MotoPro\USER\SRC\tim3.c
 */
#include "tim3.h"
#include "pid.h"
#include "can2.h"
#include "motor.h"
#include "queue.h"
void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); ///使能TIM3时钟

    TIM_TimeBaseInitStructure.TIM_Period = 11999;                   //自动重装载值11999
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7;                    //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); //初始化TIM3

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //允许定时器3更新中断
    TIM_Cmd(TIM3, ENABLE);                     //使能定时器3

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           //定时器3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //溢出中断
    {
#ifdef USE_DJ
        pulse_caculate();
        for (int id = 0; id < 8; id++)
        {
            if (motor[id].enable)
            {
                if (motor[id].begin)
                {
                    switch (motor[id].mode)
                    {
                    case current:;
                        break;
                    case RPM:
                        speed_mode(id);
                        break; //速度模式
                    case position:
                        position_mode(id);
                        break; //位置模式
                    case zero:
                        zero_mode(id);
                        break; //寻零模式
                    default:
                        break;
                    }
                }
                else
                    position_mode(id);
            }
        }
        peakcurrent(); //DJ电流限制

        SetM3508_1(motor[0].valueSet.current, motor[1].valueSet.current, motor[2].valueSet.current, motor[3].valueSet.current);
        SetM3508_2(motor[4].valueSet.current, motor[5].valueSet.current, motor[6].valueSet.current, motor[7].valueSet.current);
#endif
#ifdef USE_VESC
        for (int i = 0; i < 8; i++)
        {
            if (VESCmotor[i].enable)
            {
                if (VESCmotor[i].begin)
                {
                    switch (VESCmotor[i].mode)
                    { /*********************************************/

                    case current:
                        VESC_Set_Current(i + 1, VESCmotor[i].valSet.current, 0);
                        break;
                    case duty:
                        VESC_Set_Duty_Cycle(i + 1, VESCmotor[i].valSet.duty, 0);
                        break;
                    case RPM:
                        VESC_Set_Speed(i + 1, VESCmotor[i].valSet.speed * VESCmotor[i].instrinsic.POLE_PAIRS, 0);
                        break;
                    case brake:
                        VESC_Set_Brake_Current(i + 1, VESCmotor[i].limit.breakCurrent, 0);
                        break;
                    default:
                        break;
                    }
                }
                else
                    VESC_Set_Brake_Current(i + 1, VESCmotor[i].limit.breakCurrent, 0);
            }
            else
                VESC_Set_Current(i + 1, 0.0, 0); //发送电流
        }
#endif
        Can_DeQueue(CAN2, &Can2_Sendqueue); //ELMO EPOS
        Can_DeQueue(CAN1, &Can1_Sendqueue); //主控
        Can_DeQueue(CAN2, &VESC_Sendqueue); //VESC

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除中断标志位
    }
}
