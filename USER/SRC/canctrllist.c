/*
 * @Descripttion: 报文控制表
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-10 08:21:26
 * @FilePath: \MotoPro\USER\SRC\canctrllist.c
 */
#include "canctrllist.h"

CAN_ProcedureTypeDef CAN_Procedure;

/**
 * @author: 叮咚蛋
 * @brief: 初始化报文控制表
 * @param MesgControlGrpTypeDef
 */

void InitCANControlList(MesgControlGrpTypeDef *CAN_MesgSentList, u8 CAN_x)
{
	u8 CAN_NodeNumber = 0;
	if (CAN_1 == CAN_x)
	{
		flag.Can1CtrlList_Enable = 1;
		CAN_NodeNumber = CAN1_NodeNumber;
	}
	else
	{
		flag.Can2CtrlList_Enable = 1;
		CAN_NodeNumber = CAN2_NodeNumber;
	}
	for (int i = 0; i < CAN_NodeNumber; i++)
	{
		CAN_MesgSentList[i].SendSem = 0;
		CAN_MesgSentList[i].TimeOut = 0;
		CAN_MesgSentList[i].SendNumber = 0;
		CAN_MesgSentList[i].ReceiveNumber = 0;
		CAN_MesgSentList[i].SentQueue.Rear = 0;
		CAN_MesgSentList[i].SentQueue.Front = 0;
		CAN_MesgSentList[i].QUEUEFullTimeout = 0;
		CAN_MesgSentList[i].SentQueue.Can_DataSend[0].InConGrpFlag = 0;
	}
}

/**
 * @author: 叮咚蛋
 * @brief: 遍历报文发送状态
 * @param MesgControlGrpTypeDef 
 * @param Can_QueueTypeDef
 */

void TraversalControlList(MesgControlGrpTypeDef *CAN_MesgSentList, Can_QueueTypeDef *can_queue, u8 CAN_x)
{
	u8 CAN_NodeNumber = 0;
	if (CAN_1 == CAN_x)
		CAN_NodeNumber = CAN1_NodeNumber;
	else
		CAN_NodeNumber = CAN2_NodeNumber;
	/*遍历报文控制表*/
	for (int i = 0; i < CAN_NodeNumber; i++)
	{
		if (CAN_MesgSentList[i].SendSem >= 1)
		{
			if (CAN_MesgSentList[i].TimeOut++ >= 30)
			{
				if (QueueFull(can_queue))
				{
					CAN_MesgSentList[i].QUEUEFullTimeout++;
					return;
				}
				else if (QueueEmpty(&CAN_MesgSentList[i].SentQueue))
				{
					CAN_MesgSentList[i].QUEUEFullTimeout = 0;
					return;
				}
				else
				{
					CAN_MesgSentList[i].SendSem -= 1;
					CAN_MesgSentList[i].TimeOut = 0;
					CAN_MesgSentList[i].QUEUEFullTimeout = 0;
					can_queue->Can_DataSend[can_queue->Rear].ID = CAN_MesgSentList[i].SentQueue.Can_DataSend[CAN_MesgSentList[i].SentQueue.Front].ID;
					can_queue->Can_DataSend[can_queue->Rear].DLC = CAN_MesgSentList[i].SentQueue.Can_DataSend[CAN_MesgSentList[i].SentQueue.Front].DLC;
					can_queue->Can_DataSend[can_queue->Rear].InConGrpFlag = CAN_MesgSentList[i].SentQueue.Can_DataSend[CAN_MesgSentList[i].SentQueue.Front].InConGrpFlag;
					memcpy(can_queue->Can_DataSend[can_queue->Rear].Data, CAN_MesgSentList[i].SentQueue.Can_DataSend[CAN_MesgSentList[i].SentQueue.Front].Data,
						   sizeof(u8) * can_queue->Can_DataSend[can_queue->Rear].DLC);
				}
				can_queue->Rear = (can_queue->Rear + 1) % CAN_QUEUESIZE;
				CAN_MesgSentList[i].SentQueue.Front = (CAN_MesgSentList[i].SentQueue.Front + 1) % CAN_QUEUESIZE;
			}
			if (CAN_MesgSentList[i].QUEUEFullTimeout > 10 || (CAN_MesgSentList[i].SendNumber - CAN_MesgSentList[i].ReceiveNumber) > 50)
			{
				BEEP_ON;
				CAN_MesgSentList[i].SendSem = -10000;
				if (CAN_1 == CAN_x)
				{
					switch (i)
					{
						;
					}
				}
				else
				{
					switch (i)
					{
					case 0:
						insertError(Eerror.head, SYSTEMERROR | (1 << 4) | (2 << 8));
						break;
					case 1:
						insertError(Eerror.head, SYSTEMERROR | (1 << 4) | (2 << 8));
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

/**
 * @author: 叮咚蛋
 * @brief: 对应报文放入控制块中
 */

void Can_MesgCtrlList(MesgControlGrpTypeDef *CAN_MesgSentList, Can_QueueTypeDef *can_queue, u8 CAN_x)
{
	u8 CAN_NodeNumber = 0;
	u16 ControlListID = 0xFF;

	if (can_queue->Can_DataSend[can_queue->Front].InConGrpFlag == 0)
		return;
	if (CAN_1 == CAN_x)
	{
		CAN_NodeNumber = CAN1_NodeNumber;
	}
	else
	{
		CAN_NodeNumber = CAN2_NodeNumber;
		ControlListID = can_queue->Can_DataSend[can_queue->Front].ID - 0x301; /*报文控制块映射*/
	}
	if (ControlListID < CAN_NodeNumber) //临时屏蔽某板子只需在此处增加 &&ControlListID ！= X
	{
		if (QueueFull(&CAN_MesgSentList[ControlListID].SentQueue))
		{
			if (CAN_1 == CAN_x)
				flag.Can1SendqueueFULL++;
			else
				flag.Can2SendqueueFULL++;
		}
		else
		{
			CAN_MesgSentList[ControlListID].SendSem++;
			CAN_MesgSentList[ControlListID].SendNumber++;
			CAN_MesgSentList[ControlListID].SentQueue.Can_DataSend[CAN_MesgSentList[ControlListID].SentQueue.Rear].ID = can_queue->Can_DataSend[can_queue->Front].ID;
			CAN_MesgSentList[ControlListID].SentQueue.Can_DataSend[CAN_MesgSentList[ControlListID].SentQueue.Rear].DLC = can_queue->Can_DataSend[can_queue->Front].DLC;
			CAN_MesgSentList[ControlListID].SentQueue.Can_DataSend[CAN_MesgSentList[ControlListID].SentQueue.Rear].InConGrpFlag = can_queue->Can_DataSend[can_queue->Front].InConGrpFlag;
			memcpy(CAN_MesgSentList[ControlListID].SentQueue.Can_DataSend[CAN_MesgSentList[ControlListID].SentQueue.Rear].Data, can_queue->Can_DataSend[can_queue->Front].Data,
				   CAN_MesgSentList[ControlListID].SentQueue.Can_DataSend[CAN_MesgSentList[ControlListID].SentQueue.Rear].DLC * sizeof(u8));
			CAN_MesgSentList[ControlListID].SentQueue.Rear = (CAN_MesgSentList[ControlListID].SentQueue.Rear + 1) % CAN_QUEUESIZE;
		}
	}
}
