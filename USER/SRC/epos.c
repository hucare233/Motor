/*
 * @Descripttion: EPOS电机
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-03 20:03:56
 * @FilePath: \MotoPro\USER\SRC\epos.c
 */
#include "epos.h"
#include "macro.h"
bool Abs = 0;
bool Rel = 1;
s32 PVMspeed;
s32 PPMposition;
s32 PPMspeed;
s32 mode;
eposflag EPOSflag;
EPOSParam FLat90;
EPOSLimit EPOSlimit;
EPOSArgum EPOSargum;
EPOS_Motor EPOSmotor[5];

/**
 * @author: 叮咚蛋
 * @brief: EPOS初始化
 * @param EPOSParam
 * @param EPOSLimit
 * @param EPOSargum
 */

void EPOS_Motor_Init(void)
{
	{ //电机内参
		FLat90.PULSE = 4096;
		FLat90.RATIO = 1;
	}
	{ //电机限制
		EPOSlimit.isPosLimitON = false;
		EPOSlimit.maxAngle = 720.f;
		EPOSlimit.isPosSPLimitOn = true;
		EPOSlimit.posSPlimit = 500;
		EPOSlimit.isRealseWhenStuck = true;
		EPOSlimit.zeroSP = 200;
		EPOSlimit.zeroCurrent = 4000;
	}
	{
		EPOSargum.timeoutTicks = 2000; //200ms
	}
	{
		EPOSmotor[0].intrinsic = FLat90;
		EPOSmotor[0].enable = DISABLE;
		EPOSmotor[0].mode = PVM;
		EPOSmotor[0].valSet.angle = 300;
		EPOSmotor[0].valSet.speed = 100;
		EPOSmotor[0].valSet.current = 100;
		EPOSmotor[0].valSet.pulse = 0;

		EPOSmotor[1].intrinsic = FLat90;
		EPOSmotor[1].enable = DISABLE;
		EPOSmotor[1].mode = PVM;
		EPOSmotor[1].valSet.angle = 300;
		EPOSmotor[1].valSet.speed = 100;
		EPOSmotor[1].valSet.current = 100;
		EPOSmotor[1].valSet.pulse = 0;

		EPOSmotor[2].intrinsic = FLat90;
		EPOSmotor[2].enable = DISABLE;
		EPOSmotor[2].mode = PVM;
		EPOSmotor[2].valSet.angle = 300;
		EPOSmotor[2].valSet.speed = 100;
		EPOSmotor[2].valSet.current = 100;
		EPOSmotor[2].valSet.pulse = 0;

		EPOSmotor[3].intrinsic = FLat90;
		EPOSmotor[3].enable = DISABLE;
		EPOSmotor[3].mode = PVM;
		EPOSmotor[3].valSet.angle = 300;
		EPOSmotor[3].valSet.speed = 100;
		EPOSmotor[3].valSet.current = 100;
		EPOSmotor[3].valSet.pulse = 0;
	}
	{
		EPOSflag.bgppm = 0;
		EPOSflag.bgpvm = 0;
		EPOSflag.disable = 0;
		EPOSflag.enable = 0;
		EPOSflag.halt = 0;
		EPOSflag.quickstop = 0;
		EPOSflag.setppmposition = 0;
		EPOSflag.setppmspeed = 0;
		EPOSflag.setpvmspeed = 0;
		EPOSflag.start = 0;
		EPOSflag.um = 0;
		EPOSflag.relieve = 0;
	}
}

/**
 * @author: 叮咚蛋
 * @brief: EPOS can初始化
 */

void EPOS_Init(CAN_TypeDef *CANx)
{

	CanTxMsg tx_message;
	tx_message.StdId = 0x000;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Standard;
	tx_message.DLC = 8;
	tx_message.Data[0] = 0x01;
	tx_message.Data[1] = 0x00;
	tx_message.Data[2] = 0x00;
	tx_message.Data[3] = 0x00;
	tx_message.Data[4] = 0x00;
	tx_message.Data[5] = 0x00;
	tx_message.Data[6] = 0x00;
	tx_message.Data[7] = 0x00;
	CAN_Transmit(CANx, &tx_message);
}

/*
 * @description: set EPOS mode of operation
 * @param: ID:      node-ID
 *         mode:    operation mode
 *                  1  ->  Proofile Position Mode (PPM)
 *                  3  ->  Profile Velocity Mode (PVM)
 *                  6  ->  Homing Mode (HMM)
 *                  8  ->  Cyclic Synchronous Position Mode (CSP)
 *                  9  ->  Cyclic Synchoronous Velocity Mode (CSV)
 *                  10 ->  Cyclic Synchronous Torque Mode (CST)
 *          InConGrpFlag: put the CAN message in the control group
 */
void EPOS_SetMode(u8 ID, s32 mode, u8 InConGrpFlag)
{

	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = mode;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: torque on motor
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_StartMotor(u8 ID, u8 InConGrpFlag)
{
	//Ready to switch on

	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x06;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	//Switched on
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x07;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	//Operation enabled
	EPOS_EnableOperation(ID, InConGrpFlag);
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

void EPOS_AGAINMotorPPM(u8 ID, u8 InConGrpFlag) //启动PPM操作
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		if (Abs) //绝对位置模式
		{
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x3F;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		}
		else if (Rel) //相对位置模式
		{
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x7F;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
			Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		}

		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;

	EPOS_EnableOperation(ID, InConGrpFlag); //每次重新启动需要重发一遍0x0f
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}
/*
 * @descrription: Diable power section
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_RelieveMotor(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: set target velocity (PVM)
 * @param: ID:  node-ID
 *         speed: target velocity
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_SetPVMspeed(u8 ID, s32 speed, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0xFF;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		EncodeS32Data(&speed, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: Enable drive function (enable current controller and, if needed, positon or velocity controller)
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_EnableOperation(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x0F;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: Quick stop movement
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_QuickStop(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x20;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: Stop axis
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Halt(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x0F;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x01;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}
/*
 * @descrription: Set PPM speed
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_SetPPMspeed(u8 ID, s32 speed, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x81;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		EncodeS32Data(&speed, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: Set PPM position
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_SetPPMposition(u8 ID, s32 position, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x22;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x7A;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		EncodeS32Data(&position, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}
/*
 * @descrription: ASK demand  position
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Askdemandpos(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x62;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: ASK actual  position
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Askdactualpos(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x64;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: ASK demand speed
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Askdemandspeed(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x6B;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: ASK actual speed
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Askactualspeed(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x6C;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: ASK mode
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Askmode(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: ASK enable or disable
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Askenable_or_disable(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/*
 * @descrription: ASK torque
 * @param: ID:  node-ID
 *         InConGrpFlag: put the CAN message in the control group
 */
void EPOS_Asktorque(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x77;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	EPOSmotor[ID - 1].argum.timeout = 1;
	EPOSmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

void EPOS_ReadStatusword(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x600 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x40;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 0x41;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0x60;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
}

void EPOS_BootUP(u8 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x700 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x01;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 0x00;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
}

void EPOS_CONTROL(u8 id)
{
	id = id;
	if (EPOSflag.um)
	{
		EPOS_SetMode(1, mode, 1);
		EPOS_SetMode(2, mode, 1);
		EPOS_SetMode(3, mode, 1);
		EPOS_SetMode(4, mode, 1);
		EPOSflag.um = 0;
	}
	if (mode == 3) //速度模式
	{
		if (EPOSflag.setpvmspeed)
		{
			EPOS_SetPVMspeed(1, PVMspeed, 1);
			EPOS_SetPVMspeed(2, PVMspeed, 1);
			EPOS_SetPVMspeed(3, PVMspeed, 1);
			EPOS_SetPVMspeed(4, PVMspeed, 1);
			EPOSflag.setpvmspeed = 0;
		}
		if (EPOSflag.start)
		{
			EPOS_StartMotor(1, 1);
			EPOS_StartMotor(2, 1);
			EPOS_StartMotor(3, 1);
			EPOS_StartMotor(4, 1);
			EPOSflag.start = 0;
		}
		if (EPOSflag.enable)
		{
			EPOS_EnableOperation(1, 1);
			EPOS_EnableOperation(2, 1);
			EPOS_EnableOperation(3, 1);
			EPOS_EnableOperation(4, 1);
			EPOSflag.enable = 0;
		}
		if (EPOSflag.halt)
		{
			EPOS_Halt(1, 1);
			EPOS_Halt(2, 1);
			EPOS_Halt(3, 1);
			EPOS_Halt(4, 1);
			EPOSflag.halt = 0;
		}
	}
	else if (mode == 1) //位置模式
	{
		if (EPOSflag.setppmspeed)
		{
			EPOS_SetPPMspeed(1, PPMspeed, 1);
			EPOS_SetPPMspeed(2, PPMspeed, 1);
			EPOS_SetPPMspeed(3, PPMspeed, 1);
			EPOS_SetPPMspeed(4, PPMspeed, 1);
			EPOSflag.setppmspeed = 0;
		}
		if (EPOSflag.setppmposition)
		{
			EPOS_SetPPMposition(1, PPMposition, 1);
			EPOS_SetPPMposition(2, PPMposition, 1);
			EPOS_SetPPMposition(3, PPMposition, 1);
			EPOS_SetPPMposition(4, PPMposition, 1);
			EPOSflag.setppmposition = 0;
			;
		}
		if (EPOSflag.start)
		{
			EPOS_StartMotor(1, 1);
			EPOS_StartMotor(2, 1);
			EPOS_StartMotor(3, 1);
			EPOS_StartMotor(4, 1);
			EPOSflag.start = 0;
		}
		if (EPOSflag.enable)
		{
			EPOS_AGAINMotorPPM(1, 1);
			EPOS_AGAINMotorPPM(2, 1);
			EPOS_AGAINMotorPPM(3, 1);
			EPOS_AGAINMotorPPM(4, 1);
			EPOSflag.enable = 0;
		}
		if (EPOSflag.halt)
		{
			EPOS_Halt(1, 1);
			EPOS_Halt(2, 1);
			EPOS_Halt(3, 1);
			EPOS_Halt(4, 1);
			EPOSflag.halt = 0;
		}
	}
	if (EPOSflag.disable)
	{
		EPOS_RelieveMotor(1, 1);
		EPOS_RelieveMotor(2, 1);
		EPOS_RelieveMotor(3, 1);
		EPOS_RelieveMotor(4, 1);
		EPOSflag.disable = 0;
	}
}
