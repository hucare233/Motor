/*
 * @Descripttion: elmo
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-15 17:28:22
 * @FilePath: \MotoPro\USER\SRC\elmo.c
 */
#include "elmo.h"
#include "queue.h"
ELMOParam Flat90, U10, EC_4P_30;
ELMILimit ELMOlimit;
ELMOArgum ELMOargum;
ELMO_Motor ELMOmotor[5];
ELMOflag ELMOFlag;
u32 speed;
u8 Um;
u8 enable_or_disable;
s32 Pa;
s32 Jv;
s32 Px;
s32 Motor_Emer_Code = (0x1 << 28) | (0 << 16) | (0 << 8) | 0;
/**
 * @author: 叮咚蛋
 * @brief: 电机参数初始化
 */
void ELMO_Motor_Init(void)
{
	{ //电机内参
		Flat90.PULSE = 4096;
		Flat90.RATIO = 1;
		U10.PULSE = 4096;
		U10.RATIO = 5.0f;
		EC_4P_30.PULSE = 1000;
		EC_4P_30.RATIO = 169.0f / 9;
	}
	{ //电机限制保护
		ELMOlimit.isPosLimitON = false;
		ELMOlimit.maxAngle = 720.f;
		ELMOlimit.isPosSPLimitOn = true;
		ELMOlimit.posSPlimit = 1000;
		ELMOlimit.isRealseWhenStuck = true;
		ELMOlimit.zeroSP = 500;
		ELMOlimit.zeroCurrent = 4000;
	}
	{								  //电机其他参数设置
		ELMOargum.timeoutTicks = 500; //50ms
	}
	{
		ELMOFlag.bg = 0;
		ELMOFlag.mo = 0;
		ELMOFlag.pa = 0;
		ELMOFlag.sp = 0;
		ELMOFlag.sv = 0;
		ELMOFlag.um = 0;
		ELMOFlag.jv = 0;
		ELMOFlag.st = 0;
	}
#ifdef SteeringMotor
	/****0号电机初始化****/
	ELMOmotor[0].intrinsic = U10;
	ELMOmotor[0].enable = DISABLE;
	ELMOmotor[0].begin = DISABLE;
	ELMOmotor[0].mode = position;
	ELMOmotor[0].valSet.angle = 300;
	ELMOmotor[0].valSet.speed = 100;
	ELMOmotor[0].valSet.current = 100;
	ELMOmotor[0].valSet.pulse = 0;

	ELMOmotor[1].intrinsic = U10;
	ELMOmotor[1].enable = DISABLE;
	ELMOmotor[1].begin = DISABLE;
	ELMOmotor[1].mode = position;
	ELMOmotor[1].valSet.angle = 300;
	ELMOmotor[1].valSet.speed = 100;
	ELMOmotor[1].valSet.current = 100;
	ELMOmotor[1].valSet.pulse = 0;

	ELMOmotor[2].intrinsic = U10;
	ELMOmotor[2].enable = DISABLE;
	ELMOmotor[2].begin = DISABLE;
	ELMOmotor[2].mode = position;
	ELMOmotor[2].valSet.angle = 300;
	ELMOmotor[2].valSet.speed = 100;
	ELMOmotor[2].valSet.current = 100;
	ELMOmotor[2].valSet.pulse = 0;

	ELMOmotor[3].intrinsic = U10;
	ELMOmotor[3].enable = DISABLE;
	ELMOmotor[3].begin = DISABLE;
	ELMOmotor[3].mode = position;
	ELMOmotor[3].valSet.angle = 300;
	ELMOmotor[3].valSet.speed = 100;
	ELMOmotor[3].valSet.current = 100;
	ELMOmotor[3].valSet.pulse = 0;

#elif defined ActionMotor
#ifdef PassRobot

#elif defined TryRobot
	/****0号电机初始化****/
	ELMOmotor[0].intrinsic = U10;
	ELMOmotor[0].enable = DISABLE;
	ELMOmotor[0].mode = position;
	ELMOmotor[0].valSet.angle = 300;
	ELMOmotor[0].valSet.speed = 100;
	ELMOmotor[0].valSet.current = 100;
	ELMOmotor[0].valSet.pulse = 0;
#endif
#endif

	ELMOmotor[0].limit = ELMOlimit;
	ELMOmotor[0].argum = ELMOargum;
}

/**
 * @author: 叮咚蛋
 * @brief: can初始化
 */

void ELMO_Init(CAN_TypeDef *CANx)
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

/**
 * @author: 叮咚蛋
 * @brief: 设置模式 2-速度 5-位置
 */

void Elmo_Motor_UM(u32 ID, u8 UM, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'U';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'M';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = UM;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 保存
 */

void Elmo_Motor_SV(u32 ID, u8 InConGrpFlag)
{

	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'S';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'V';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 加速度
 */

void Elmo_Motor_AC(u32 ID, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'A';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'C';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 减速度
 */

void Elmo_Motor_DC(u32 ID, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'D';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'C';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 使能或失能
 */

void Elmo_Motor_Enable_Or_Disable(u32 ID, u8 enable_or_disable, u8 InConGrpFlag) //使能或失能
{
	CanTxMsg tx_message;
	tx_message.StdId = 0X300 + ID;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Standard;
	tx_message.DLC = 0X08;
	tx_message.Data[0] = 'M';
	tx_message.Data[1] = 'O';
	tx_message.Data[2] = 0;
	tx_message.Data[3] = 0;
	tx_message.Data[4] = enable_or_disable;
	tx_message.Data[5] = 0;
	tx_message.Data[6] = 0;
	tx_message.Data[7] = 0;
	CAN_Transmit(CAN2, &tx_message);
}

/**
 * @author: 叮咚蛋
 * @brief: 开始运动
 */

void Elmo_Motor_BG(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'B';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'G';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 制动
 */

void Elmo_Motor_ST(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'S';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'T';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 位置模式持续速度
 */

void Elmo_Motor_SP(u32 ID, s32 speed, u8 InConGrpFlag) //设置转速
{
	s32 S_P;
	S_P = ELMOmotor[ID - 1].intrinsic.PULSE * 4 * speed / 60;
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'S';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&S_P, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 设置绝对位置
 */

void Elmo_Motor_PA(u32 ID, s32 PA, u8 InConGrpFlag)
{

	s32 P_a;
	P_a = ELMOmotor[ID - 1].intrinsic.PULSE * 4 * PA * ELMOmotor[ID - 1].intrinsic.RATIO / 360.f;
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'A';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&P_a, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 设置相对位置
 */

void Elmo_Motor_PR(u32 ID, s32 PR, u8 InConGrpFlag)
{

	s32 P_r;
	P_r = ELMOmotor[ID - 1].intrinsic.PULSE * 4 * PR * ELMOmotor[ID - 1].intrinsic.RATIO / 360.f;
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'R';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&P_r, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 设置相对位置
 */

void Elmo_Motor_PX(u32 ID, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'X';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 速度模式持续速度
 */

void Elmo_Motor_JV(u32 ID, s32 JV, u8 InConGrpFlag)
{

	s32 J_v;
	J_v = ELMOmotor[ID - 1].intrinsic.PULSE * 4 * JV / 60.f;

	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'J';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'V';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&J_v, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: PVT启动前
 */

void Elmo_Motor_RM(u32 ID, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'R';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'M';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: PVT模式
 */

void Elmo_Motor_PV(u32 ID, u8 PV, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'V';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = PV;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 设置PVT数组  TODO: QP数组的给值要好好研究
 */

void Elmo_Motor_MP(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'M';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = Sub;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 设置位置数组
 */

void Elmo_Motor_QP(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'Q';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = Sub;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 设置时间数组
 */

void Elmo_Motor_QT(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'Q';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'T';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = Sub;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 设置速度数组
 */

void Elmo_Motor_QV(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'Q';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'V';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = Sub;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		EncodeS32Data(&data, &Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4]);
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: PT模式   //采样时间在MP[4]更改
 */

void Elmo_Motor_PT(u32 ID, u8 PT, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0x08;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'T';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[4] = PT;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[5] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[6] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[7] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 查询实际转速
 */

void Elmo_Motor_ASKvx(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'V';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'X';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 查询电流
 */

void Elmo_Motor_ASKiq(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'I';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'Q';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 查询位置
 */

void Elmo_Motor_ASKpa(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'A';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}
/**
 * @author: 叮咚蛋
 * @brief: 查询加速度
 */

void Elmo_Motor_ASKac(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'A';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'C';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 查询减速度
 */

void Elmo_Motor_ASKdc(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'D';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'C';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}
/**
 * @author: 叮咚蛋
 * @brief: 查询相对位置
 */

void Elmo_Motor_ASKpx(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'X';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 查询位置转速
 */

void Elmo_Motor_ASKsp(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'S';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'P';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 查询使能
 */

void Elmo_Motor_ASKmo(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'M';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'O';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

/**
 * @author: 叮咚蛋
 * @brief: 查询模式
 */

void Elmo_Motor_ASKum(u32 ID, u8 InConGrpFlag)
{
	if (Rear2 == Can2_Sendqueue.Front)
	{
		flag.Can2SendqueueFULL++;
		return;
	}
	else
	{
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].ID = 0x300 + ID;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].DLC = 0X04;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[0] = 'U';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[1] = 'M';
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[2] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].Data[3] = 0;
		Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Rear].InConGrpFlag = InConGrpFlag;
	}
	Can2_Sendqueue.Rear = Rear2;
	ELMOmotor[ID - 1].argum.timeout = 1;
	ELMOmotor[ID - 1].argum.lastRxTim = OSTimeGet();
}

void elmo_control(u32 id)
{
	if (ELMOFlag.um)
	{
		Elmo_Motor_UM(id, Um, 1);
		ELMOFlag.um = 0;
	}
	//	Delay_ms(50);
	if (ELMOFlag.mo)
	{
		Elmo_Motor_Enable_Or_Disable(id, enable_or_disable, 1);
		enable_or_disable = 0;
		ELMOFlag.mo = 0;
	}
	if (ELMOFlag.bg)
	{
		Elmo_Motor_BG(id, 1);
		ELMOFlag.bg = 0;
	}
	//Delay_ms(50);
	if (ELMOFlag.jv)
	{
		Elmo_Motor_JV(id, Jv, 1);
		ELMOFlag.jv = 0;
	}
	if (ELMOFlag.pa)
	{
		Elmo_Motor_PA(id, Pa, 1);
		ELMOFlag.pa = 0;
	}
	if (ELMOFlag.sp)
	{
		Elmo_Motor_SP(id, speed, 1);
		ELMOFlag.sp = 0;
	}
	if (ELMOFlag.st)
	{
		Elmo_Motor_ST(id, 1);
		ELMOFlag.st = 0;
	}
	if (ELMOFlag.sv)
	{
		Elmo_Motor_SV(id, 1);
		ELMOFlag.sv = 0;
	}
}
