#include "common_can.h"

/****Start Remote Node Protocol****/
void NMT_Operational(CAN_TypeDef* CANx)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x000;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Standard;
	tx_message.DLC=8;
	tx_message.Data[0]=0x01;
	tx_message.Data[1]=0x00;
	tx_message.Data[2]=0x00;
	tx_message.Data[3]=0x00;
	tx_message.Data[4]=0x00;
	tx_message.Data[5]=0x00;
	tx_message.Data[6]=0x00;
	tx_message.Data[7]=0x00;
	CAN_Transmit(CANx,&tx_message);
}

/****Enter pre-operational****/
void NMT_PreOperational(CAN_TypeDef* CANx)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x000;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Standard;
	tx_message.DLC=8;
	tx_message.Data[0]=0x80;
	tx_message.Data[1]=0x00;
	tx_message.Data[2]=0x00;
	tx_message.Data[3]=0x00;
	tx_message.Data[4]=0x00;
	tx_message.Data[5]=0x00;
	tx_message.Data[6]=0x00;
	tx_message.Data[7]=0x00;
	CAN_Transmit(CANx,&tx_message);
}

/****Stop remote Node Protocol****/
void NMT_Stopped(CAN_TypeDef* CANx)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x000;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Standard;
	tx_message.DLC=8;
	tx_message.Data[0]=0x02;
	tx_message.Data[1]=0x00;
	tx_message.Data[2]=0x00;
	tx_message.Data[3]=0x00;
	tx_message.Data[4]=0x00;
	tx_message.Data[5]=0x00;
	tx_message.Data[6]=0x00;
	tx_message.Data[7]=0x00;
	CAN_Transmit(CANx,&tx_message);
}

/****Reset node****/
void NMT_ResetApplication(CAN_TypeDef* CANx)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x000;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Standard;
	tx_message.DLC=8;
	tx_message.Data[0]=0x81;
	tx_message.Data[1]=0x00;
	tx_message.Data[2]=0x00;
	tx_message.Data[3]=0x00;
	tx_message.Data[4]=0x00;
	tx_message.Data[5]=0x00;
	tx_message.Data[6]=0x00;
	tx_message.Data[7]=0x00;
	CAN_Transmit(CANx,&tx_message);
}

/****Reset communication****/
void NMT_Communication(CAN_TypeDef* CANx)
{
	CanTxMsg tx_message;
	tx_message.StdId=0x000;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Standard;
	tx_message.DLC=8;
	tx_message.Data[0]=0x82;
	tx_message.Data[1]=0x00;
	tx_message.Data[2]=0x00;
	tx_message.Data[3]=0x00;
	tx_message.Data[4]=0x00;
	tx_message.Data[5]=0x00;
	tx_message.Data[6]=0x00;
	tx_message.Data[7]=0x00;
	CAN_Transmit(CANx,&tx_message);
}
