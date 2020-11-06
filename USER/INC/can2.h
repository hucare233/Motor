#ifndef __CAN2_H
#define __CAN2_H
#include "sys.h"
#include "stm32f4xx.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_rcc.h"
#include "motor.h"
#include "epos.h"
#include "queue.h"
#define CAN1_RX0_INT_ENABLE 1                                   //0,不使能;1,使能.
u8 CAN2_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode); //CAN初始化
u8 CAN2_Send_Msg(u8 *msg, u8 len);                              //发送数据
u8 CAN2_Receive_Msg(u8 *buf);                                   //接收数据
extern u32 last_update_time[8];
extern u32 now_update_time[8];
extern s16 timeout_counts;
extern short Velocity_3508_ID7;
extern short Position_3508_ID7;
extern short Torque_3508_ID7;
extern unsigned char can_tx_success_flag;
#endif
