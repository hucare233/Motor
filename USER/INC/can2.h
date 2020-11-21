/*
 * @Descripttion: 
 * @version: ��һ��
 * @Author: ���˵�
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-11-21 22:48:12
 * @FilePath: \MotoPro\USER\INC\can2.h
 */
#ifndef __CAN2_H
#define __CAN2_H
#include "sys.h"
#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_rcc.h"
#include "motor.h"
#include "epos.h"
#include "queue.h"
#define CAN1_RX0_INT_ENABLE 1                                   //0,��ʹ��;1,ʹ��.
u8 CAN2_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode); //CAN��ʼ��
u8 CAN2_Send_Msg(u8 *msg, u8 len);                              //��������
u8 CAN2_Receive_Msg(u8 *buf);   
void valveCtrl(bool status);
void currentInput(u8 id);//��������
extern u32 last_update_time[8];
extern u32 now_update_time[8];
extern s16 timeout_counts;
extern short Velocity_3508_ID7;
extern short Position_3508_ID7;
extern short Torque_3508_ID7;
extern unsigned char can_tx_success_flag;
#endif
