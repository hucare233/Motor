/*
 * @Descripttion: 
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-10 08:22:57
 * @FilePath: \MotoPro\USER\INC\queue.h
 */
#ifndef _QUEUE_H
#define _QUEUE_H

#include "param.h"
#include "string.h"
#include "stm32f4xx_can.h"
#include <stdbool.h>
#define CAN_1 (u8)(0x01 << 0)
#define CAN_2 (u8)(0x01 << 1)

/**
  *@brief 报文控制表
  */
#define CAN_QUEUESIZE 30		//发送报文队列长度
#define CAN_HAVESENDQUEUESIZE 5 //已发送报文队列长度
#define CAN1_NodeNumber 6u		//不要超过8（故障标志位只有8位）
#define CAN2_NodeNumber 6u		//不要超过8（故障标志位只有8位）
#define Rear1 ((Can1_Sendqueue.Rear + 1) % CAN_QUEUESIZE)
#define Rear2 ((Can2_Sendqueue.Rear + 1) % CAN_QUEUESIZE)
#define Rear3 ((VESC_Sendqueue.Rear + 1) % CAN_QUEUESIZE)
/****CAN报文发送结构体****/
typedef struct
{
	u32 ID;
	u8 DLC;
	u8 Data[8];
	u8 InConGrpFlag;
} Can_SendtructTypeDef;

/****CAN报文队列****/
typedef struct
{
	uint16_t Front, Rear;
	Can_SendtructTypeDef Can_DataSend[CAN_QUEUESIZE];
} Can_QueueTypeDef;

/****报文控制块****/
typedef struct
{
	s32 SendNumber;
	s32 ReceiveNumber;
	u32 QUEUEFullTimeout;
	u8 TimeOut;
	s32 SendSem;
	Can_QueueTypeDef SentQueue;
} MesgControlGrpTypeDef;

/****CAN流程号****/
typedef struct
{
	volatile u8 order_valid; //当前有效报文流程号
	volatile u8 order_now;	 //当前报文流程号
	volatile u8 status;		 //当前流程状态，
} Can_ProcedureTypeDef;

extern Can_ProcedureTypeDef Can_Procedure;
extern Can_SendtructTypeDef Can_Sendtruct;
extern Can_QueueTypeDef Can_Sentqueue, Can1_Sendqueue, Can2_Sendqueue, VESC_Sendqueue;
extern MesgControlGrpTypeDef Can1_MesgSentList[CAN1_NodeNumber],Can2_MesgSentList[CAN2_NodeNumber], Can2_elmoMesgSentList[CAN2_NodeNumber], Can2_eposMesgSentList[CAN2_NodeNumber];

void Can_SendqueueInit(void);
bool QueueEmpty(Can_QueueTypeDef *can_queue);
bool QueueFull(Can_QueueTypeDef *can_queue);
void Can_DeQueue(CAN_TypeDef *CANx, Can_QueueTypeDef *can_queue);
void InitCANControlList(MesgControlGrpTypeDef *CAN_MesgSentList, u8 CAN_x);
void Can_MesgCtrlList(MesgControlGrpTypeDef *CAN_MesgSentList, Can_QueueTypeDef *can_queue, u8 CAN_x);
void TraversalControlList(MesgControlGrpTypeDef *CAN_MesgSentList, Can_QueueTypeDef *can_queue, u8 CAN_x);

#endif
