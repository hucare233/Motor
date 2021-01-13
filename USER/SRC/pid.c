/*
 * @Descripttion: pid控制
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2021-01-13 22:31:08
 * @FilePath: \MotoPro\USER\SRC\pid.c
 */
#include "pid.h"
#include "can2.h"

/**
 * @author: 叮咚蛋
 * @brief: pid初始化
 * @param PID_setTypeDef
 */

void PID_Init(PID_setTypeDef *PID, float KP, float KI, float KD, float KS, s32 Set)
{
	PID->CurVal = 0;
	PID->SetVal = Set;
	PID->liEkVal[0] = 0;
	PID->liEkVal[1] = 0;
	PID->liEkVal[2] = 0;
	PID->uKD_Coe = KD;
	PID->uKP_Coe = KP;
	PID->uKI_Coe = KI;
	PID->uKS_Coe = KS;
}

/**
 * @author: 叮咚蛋
 * @brief: 本杰明pid初始化
 * @param {float} KP
 * @param {float} KI
 * @param {float} KD
 * @param {float} KD_Filter
 */

void VESC_PID_Init(VESC_PID_setTypeDef* PID, float KP,float KI,float KD,float KD_Filter)
{
  PID->kp = KP;
  PID->ki = KI;
  PID->kd = KD;
  PID->kd_filter = KD_Filter;
}

/**
 * @author: 叮咚蛋
 * @brief: pid运算
 * @param {s32} setval
 * @param {s32} curval
 */

void VESC_PID_Operation(VESC_PID_setTypeDef* PID, s32 setval, s32 curval)
{
  PID->SetVal = setval;
  PID->CurVal = curval;
  PID->error = PID->SetVal - PID->CurVal;
  
  // Too low RPM set. Reset state and return.
	if (ABS(PID->SetVal) < 20) {//TODO: 此处20暂乱给
		PID->i_term = 0.0f;
		PID->prv_error = PID->error;
		return;
	}
  
  // Compute parameters
  PID->p_term = PID->error * PID->kp / 100.f;
  PID->i_term += PID->error * PID->ki / 1000.f / 100.f;
  PID->d_term = (PID->error - PID->prv_error) * PID->kd / 100.f;//TODO:应该*1000，方便计算省略
  
  // Filter D
  UTILS_LP_FAST(PID->d_filter, PID->d_term, PID->kd_filter);
	PID->d_term = PID->d_filter;  
  
 	// I-term wind-up protection
	utils_truncate_number(&PID->i_term, -1.0f, 1.0f);  
  
  // Store previous error
	PID->prv_error = PID->error;
  
	// Calculate output
	PID->output = PID->p_term + PID->i_term + PID->d_term;
	utils_truncate_number(&PID->output, -1.0f, 1.0f);  
}

/**
 * @author: 叮咚蛋
 * @brief: 进行pid运算
 * @param PID_setTypeDef
 */

void PID_Operation(PID_setTypeDef *PID)
{
	PID->liEkVal[0] = PID->SetVal - PID->CurVal;
	PID->Udlt = PID->uKP_Coe * (PID->liEkVal[0] - PID->liEkVal[1]) + PID->uKI_Coe * PID->liEkVal[0] + PID->uKD_Coe * (PID->liEkVal[0] + PID->liEkVal[2] - PID->liEkVal[1] * 2);
	PID->liEkVal[2] = PID->liEkVal[1];
	PID->liEkVal[1] = PID->liEkVal[0];
}

/**
 * @author: 叮咚蛋
 * @brief: 发送DJ id 5-7
 */

void SetM3508_2(short ID5, short ID6, short ID7, short ID8) //发送5-8电机
{

	unsigned short can_id = 0x000;
	CanTxMsg tx_message;

	tx_message.IDE = CAN_ID_STD;   //标准帧
	tx_message.RTR = CAN_RTR_DATA; //数据帧
	tx_message.DLC = 0x08;		   //8位
	can_id = 0x1FF;
	tx_message.StdId = can_id;
	if (!ifstuck(4) && (motor[4].enable == 1))
	{
		tx_message.Data[0] = (ID5 >> 8);
		tx_message.Data[1] = ID5; //ID 1
	}
	else
	{
		tx_message.Data[0] = 0;
		tx_message.Data[1] = 0;
	}
	if (!ifstuck(5) && (motor[5].enable == 1))
	{
		tx_message.Data[2] = (ID6 >> 8);
		tx_message.Data[3] = ID6; //ID 1
	}
	else
	{
		tx_message.Data[2] = 0;
		tx_message.Data[3] = 0;
	}
	if (!ifstuck(6) && (motor[6].enable == 1))
	{
		tx_message.Data[4] = (ID7 >> 8);
		tx_message.Data[5] = ID7; //ID 1
	}
	else
	{
		tx_message.Data[4] = 0;
		tx_message.Data[5] = 0;
	}
	if (!ifstuck(7) && (motor[7].enable == 1))
	{
		tx_message.Data[6] = (ID8 >> 8);
		tx_message.Data[7] = ID8; //ID 1
	}
	else
	{
		tx_message.Data[6] = 0;
		tx_message.Data[7] = 0;
	}
	CAN_Transmit(CAN2, &tx_message);
}

/**
 * @author: 叮咚蛋
 * @brief: 发送电流 id 1-4
 */

void SetM3508_1(short ID1, short ID2, short ID3, short ID4) //发送1-4电机
{
	unsigned short can_id = 0x000;
	CanTxMsg tx_message;

	tx_message.IDE = CAN_ID_STD;   //标准帧
	tx_message.RTR = CAN_RTR_DATA; //数据帧
	tx_message.DLC = 0x08;		   //8位
	can_id = 0x200;
	tx_message.StdId = can_id;
	if (!ifstuck(0) && (motor[0].enable == 1))
	{
		tx_message.Data[0] = (ID1 & 0xFF00) >> 8;
		tx_message.Data[1] = ID1 & 0xFF; //ID 1
	}
	else
	{
		tx_message.Data[0] = 0;
		tx_message.Data[1] = 0;
	}
	if (!ifstuck(1) && (motor[1].enable == 1))
	{
		tx_message.Data[2] = (ID2 & 0xFF00) >> 8;
		tx_message.Data[3] = ID2 & 0xFF; //ID 1
	}
	else
	{
		tx_message.Data[2] = 0;
		tx_message.Data[3] = 0;
	}
	if (!ifstuck(2) && (motor[2].enable == 1))
	{
		tx_message.Data[4] = (ID3 & 0xFF00) >> 8;
		tx_message.Data[5] = ID3 & 0xFF; //ID 1
	}
	else
	{
		tx_message.Data[4] = 0;
		tx_message.Data[5] = 0;
	}
	if (!ifstuck(3) && (motor[3].enable == 1))
	{
		tx_message.Data[6] = (ID4 & 0xFF00) >> 8;
		tx_message.Data[7] = ID4 & 0xFF; //ID 1
	}
	else
	{
		tx_message.Data[6] = 0;
		tx_message.Data[7] = 0;
	}
	CAN_Transmit(CAN2, &tx_message);
}

/**
 * @author: 叮咚蛋
 * @brief: 电流限制
 */

void peakcurrent(void)
{
	for (u8 id = 0; id < 8; id++)
		PEAK(motor[id].valueSet.current, motor[id].intrinsic.CURRENT_LIMIT);
}
