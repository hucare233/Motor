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
	u8 i = 0;
	u8 CAN_NodeNumber;
	if(CAN_1 == CAN_x)
	{
		CAN_NodeNumber = CAN1_NodeNumber;
	}
	else
	{
		CAN_NodeNumber = CAN2_NodeNumber;
	}
	
	for(i = 0; i < CAN_NodeNumber; i++)
	{
		CAN_MesgSentList[i].SendSem = 0;
		CAN_MesgSentList[i].TimeOut = 0;
		CAN_MesgSentList[i].QUEUEFullTimeout = 0;
		CAN_MesgSentList[i].ReceiveNumber = 0;
		CAN_MesgSentList[i].SendNumber = 0;
	}
}

/**
 * @author: 叮咚蛋
 * @brief: 对应报文控制表检测
 */

void Can_MesgCtrlList(MesgControlGrpTypeDef *CAN_MesgSentList, Can_QueueTypeDef *can_queue, u8 CAN_x)
{
	u16 ControlListID=0xFF;
	u8 CAN_NodeNumber;

	if( CAN_1 == CAN_x )
	{
		CAN_NodeNumber = CAN1_NodeNumber;
		switch(can_queue->Can_DataSend[can_queue->Front].ID)
		{
			case 0x303:
				ControlListID = 0;
			break;
			case 0x304:
				ControlListID = 1;
			break;
			case 0x305:
				ControlListID = 2;
			break;
			default:
			break;
		}
		if(ControlListID<CAN_NodeNumber)
		{
			CAN_MesgSentList[ControlListID].SendSem ++;
			CAN_MesgSentList[ControlListID].SendNumber ++;
			CAN_MesgSentList[ControlListID].TimeOut ++;
			if(CAN_MesgSentList[ControlListID].TimeOut>10)
			{
				flag.Can1_ErrNode|=(1<<ControlListID);
			}
		}
	}
	else if( CAN_2 == CAN_x )
	{
		CAN_NodeNumber = CAN2_NodeNumber;
		switch(can_queue->Can_DataSend[can_queue->Front].ID)	
		{
			case 0x300:
				ControlListID = CAN_NodeNumber + 1;
				break;
			
			case 0x301:
				ControlListID = 0;
				break;
			
			case 0x302:
				ControlListID = 1;
				break;
				
			case 0x303:
				ControlListID = 2;
				break;
			case 0x304:
				ControlListID = 3;
				break;
			
				default:
				break;
		}
		if(ControlListID<CAN_NodeNumber)
		{
			CAN_MesgSentList[ControlListID].SendSem ++;
			CAN_MesgSentList[ControlListID].SendNumber ++;
			CAN_MesgSentList[ControlListID].TimeOut ++;
			if(CAN_MesgSentList[ControlListID].TimeOut>100)
			{
				flag.Can2_ErrNode|=(1<<ControlListID);
			}
		}
  }
}
