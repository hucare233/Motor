/*
 * @Descripttion: ��ʱ���������ķ���
 * @version: �ڶ���
 * @Author: ���˵�
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: ���˵�
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); ///ʹ��TIM3ʱ��

    TIM_TimeBaseInitStructure.TIM_Period = 11999;                   //�Զ���װ��ֵ11999
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7;                    //��ʱ����Ƶ
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure); //��ʼ��TIM3

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //����ʱ��3�����ж�
    TIM_Cmd(TIM3, ENABLE);                     //ʹ�ܶ�ʱ��3

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;           //��ʱ��3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //����ж�
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
                        break; //�ٶ�ģʽ
                    case position:
                        position_mode(id);
                        break; //λ��ģʽ
                    case zero:
                        zero_mode(id);
                        break; //Ѱ��ģʽ
                    default:
                        break;
                    }
                }
                else
                    position_mode(id);
            }
        }
        peakcurrent(); //DJ��������

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
                VESC_Set_Current(i + 1, 0.0, 0); //���͵���
        }
#endif
        Can_DeQueue(CAN2, &Can2_Sendqueue); //ELMO EPOS
        Can_DeQueue(CAN1, &Can1_Sendqueue); //����
        Can_DeQueue(CAN2, &VESC_Sendqueue); //VESC

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //����жϱ�־λ
    }
}
