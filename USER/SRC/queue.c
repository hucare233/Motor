/*
 * @Descripttion: can队列
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-23 20:06:57
 * @FilePath: \MotoPro\USER\SRC\queue.c
 */
#include "queue.h"

/****相关结构体定义****/
Can_ProcedureTypeDef Can_Procedure;
Can_SendtructTypeDef Can_Sendtruct;
MesgControlGrpTypeDef Can1_MesgSentList[CAN1_NodeNumber], Can2_MesgSentList[CAN2_NodeNumber],Can2_elmoMesgSentList[CAN2_NodeNumber], Can2_eposMesgSentList[CAN2_NodeNumber];
Can_QueueTypeDef Can_Sentquene, Can1_Sendqueue, Can2_Sendqueue, VESC_Sendqueue;

/****基本数据****/
u8 CanSendCount;
int Can1_Tx_NoMailBox;
int Can2_Tx_NoMailBox;

/****判断是否队空****/
bool QueueEmpty(Can_QueueTypeDef *can_queue)
{
	return (can_queue->Front == can_queue->Rear);
}

/****判断是否队满****/
bool QueueFull(Can_QueueTypeDef *can_queue)
{
	return (((can_queue->Rear + 1) % CAN_QUEUESIZE) == can_queue->Front);
}

/****CAN队列初始化****/
void Can_SendqueueInit(void)
{
	Can1_Sendqueue.Front = Can1_Sendqueue.Rear = 0;
	Can2_Sendqueue.Front = Can2_Sendqueue.Rear = 0;
	VESC_Sendqueue.Front = VESC_Sendqueue.Rear = 0;
}

/****CAN出队函数****/
void Can_DeQueue(CAN_TypeDef *CANx, Can_QueueTypeDef *can_queue)
{
	CanTxMsg TxMessage;
	CanSendCount = 0;
	if (QueueEmpty(can_queue))
		flag.CanSendqueueEMPTY = 1;
	else
	{
		if (can_queue->Can_DataSend[can_queue->Front].ID < 0x800)  //DJ ELMO EPOS报文
		{
			TxMessage.IDE = CAN_ID_STD;
			TxMessage.StdId = can_queue->Can_DataSend[can_queue->Front].ID;
		}
		else
		{
			if ((can_queue->Can_DataSend[can_queue->Front].ID & 0xf0000000) != 0) //VESC报文的标志
			{
				can_queue->Can_DataSend[can_queue->Front].ID = can_queue->Can_DataSend[can_queue->Front].ID & 0x0fffffff;
				TxMessage.IDE = CAN_ID_EXT;
				TxMessage.ExtId = can_queue->Can_DataSend[can_queue->Front].ID;
			}
		}

		//		if(CAN1==CANx)
		// 	        Can_MesgCtrlList(Can1_MesgSentList,&Can1_Sendqueue,CAN_1);
		//		else
		//			Can_MesgCtrlList(Can2_MesgSentList,&Can2_Sendqueue,CAN_2);

		TxMessage.DLC = can_queue->Can_DataSend[can_queue->Front].DLC;
		TxMessage.RTR = CAN_RTR_DATA;
		memcpy(TxMessage.Data, (can_queue->Can_DataSend[can_queue->Front].Data), TxMessage.DLC * sizeof(uint8_t));
		can_queue->Can_DataSend[can_queue->Front].InConGrpFlag = 0; //出队flag清0
		can_queue->Front = (can_queue->Front + 1) % CAN_QUEUESIZE;

		/****保护措施****/
		while ((CAN_Transmit(CANx, &TxMessage)) == CAN_TxStatus_NoMailBox)
		{
			CanSendCount++;
			if (CanSendCount > 10)
			{
				if (CANx == CAN1)
					Can1_Tx_NoMailBox++;
				else
					Can2_Tx_NoMailBox++;
				break;
			}
		}
		if (Can1_Tx_NoMailBox >= 5)
		{
			Can1_Tx_NoMailBox = 0;
			insertError(Eerror.head, SYSTEMERROR | (1 << 4) | (1 << 8));
		}
		if (Can2_Tx_NoMailBox >= 5)
		{
			Can2_Tx_NoMailBox = 0;
			insertError(Eerror.head, SYSTEMERROR | (2 << 4) | (1 << 8));
		}
//			if(CANx == CAN2) //电机的报文
//	{
//		while ((CAN_Transmit(CAN2, &TxMessage)) == CAN_TxStatus_NoMailBox)
//		{
//			CanSendCount++;
//			if (CanSendCount > 10) break;
//		}
//		if (CanSendCount > 10)
//		{
//			Can2_Tx_NoMailBox++;
//			if(Can2_Tx_NoMailBox>10)
//				insertError(Eerror.head, SYSTEMERROR | (2 << 4) | (1 << 8));
//		}
//		else
//		{
//			Can2_Tx_NoMailBox=0;
//			deleteError(Eerror.head, ErrorFind(Eerror.head, SYSTEMERROR | (2 << 4) | (1 << 8)));
//			Can_MesgCtrlList(Can2_MesgSentList,&Can2_Sendqueue,CAN_2);
//			if(Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag==false)
//				can_queue->Front = (can_queue->Front + 1) % CAN_QUEUESIZE;
//		}
//	}
//	else if(CANx == CAN1)
//	{
//		while ((CAN_Transmit(CAN1, &TxMessage)) == CAN_TxStatus_NoMailBox)
//		{
//			CanSendCount++;
//			if (CanSendCount > 10) break;
//		}
//		if (CanSendCount > 10)
//		{
//			Can1_Tx_NoMailBox++;
//			if(Can1_Tx_NoMailBox>10)
//				insertError(Eerror.head, SYSTEMERROR | (1 << 4) | (1 << 8));
//		}
//		else
//		{
//			Can1_Tx_NoMailBox=0;
//			deleteError(Eerror.head, ErrorFind(Eerror.head, SYSTEMERROR | (1 << 4) | (1 << 8)));
//			Can_MesgCtrlList(Can1_MesgSentList,&Can1_Sendqueue,CAN_1);
//			if(Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag==false)
//				can_queue->Front = (can_queue->Front + 1) % CAN_QUEUESIZE;
//		}
//	}
	}
}
