#ifndef __VESC_H
#define __VESC_H

#include "sys.h"
#include "stdint.h"
#include "param.h"
#include "queue.h"
#include "canctrllist.h"
#include "buffer.h"
#include "motor.h"
#include <math.h>
#include "utils.h"
/****VESC内参****/
typedef struct
{
	u8 POLE_PAIRS; //电机极对数
	float MAX_CURRENT;
	s32 MAX_RPM;
} VESCParam;

/****VESC外参****/
typedef struct
{
	volatile float current; //电流
	vs32 speed;				//速度
	float angle;			//位置
	vs32 position;
	float duty; //占空比
} VESCVal;

/****VESC状态****/
typedef struct
{
	bool stuck;		//电机堵转
	bool timeout;	//can报文反馈超时
	bool isSetZero; //设置当前位置为零点
} VESCStatus;

/****VESC限制保护****/
typedef struct
{
	float breakCurrent; //刹车电流
	float maxCurrentSet;
	bool isPosLimitOn;	 //位置限制
	s32 maxPosition;	 //位置限制的最大位置
	bool isPosSPLimitOn; //位置环速度限制
	s32 posSPlimit;		 //位置环下的最大速度
} VESCLimit;

/***不需要关心的一些参数*****/
typedef struct
{
	u8 timeoutCnt;	  //超时用计数值，[相关于超时状态timeout]
	u32 lastRxTim;	  //上次接收到报文的系统时间，[相关于超时状态timeout]
	u16 timeoutTicks; //判断超时用系统节拍数，[相关于超时状态timeout]
	vs16 distance;	  //两次角度的差值
	bool fistPos;	  //第一次上电角度
	vu16 angleNow;
	vu16 anglePrv;
	float lockAngle;
	s32 lockPosition;
	vs32 difPosition;
	bool timeout;
} VESCArgum;

/****VESC电机结构体****/
typedef struct
{
	bool enable;			   //电机使能
	bool begin;				   //电机启动
	bool Encoder_DIR_inverted; //旋转方向逆转
	u8 mode;				   //电机模式
	VESCVal valSet, valReal;   //外参设定值，实际值，前次值
	VESCStatus status;		   //电机状态
	PID_setTypeDef PIDx, PIDs; //PID参数
	VESC_PID_setTypeDef p_pid, s_pid;
	VESCArgum argum;	  //间值参数
	VESCLimit limit;	  //电机限制保护
	VESCParam instrinsic; //电机内参
} VESC_MOTOR;

/****VESC的can报文命令枚举体****/
typedef enum
{
	CAN_PACKET_SET_DUTY = 0,
	CAN_PACKET_SET_CURRENT,
	CAN_PACKET_SET_CURRENT_BRAKE,
	CAN_PACKET_SET_RPM,
	CAN_PACKET_SET_POS,
	CAN_PACKET_FILL_RX_BUFFER,
	CAN_PACKET_FILL_RX_BUFFER_LONG,
	CAN_PACKET_PROCESS_RX_BUFFER,
	CAN_PACKET_PROCESS_SHORT_BUFFER,
	CAN_PACKET_STATUS,
	CAN_PACKET_SET_CURRENT_REL,
	CAN_PACKET_SET_CURRENT_BRAKE_REL,
	CAN_PACKET_SET_CURRENT_HANDBRAKE,
	CAN_PACKET_SET_CURRENT_HANDBRAKE_REL,
	CAN_PACKET_STATUS_2,
	CAN_PACKET_STATUS_3,
	CAN_PACKET_STATUS_4,
	CAN_PACKET_PING,
	CAN_PACKET_PONG,
	CAN_PACKET_DETECT_APPLY_ALL_FOC,
	CAN_PACKET_DETECT_APPLY_ALL_FOC_RES,
	CAN_PACKET_CONF_CURRENT_LIMITS,
	CAN_PACKET_CONF_STORE_CURRENT_LIMITS,
	CAN_PACKET_CONF_CURRENT_LIMITS_IN,
	CAN_PACKET_CONF_STORE_CURRENT_LIMITS_IN,
	CAN_PACKET_CONF_FOC_ERPMS,
	CAN_PACKET_CONF_STORE_FOC_ERPMS,
	CAN_PACKET_STATUS_5
} CAN_PACKET_ID_Enum;

extern VESC_MOTOR VESCmotor[4];

void VESCInit(void);
void VESC_caculate(VESC_MOTOR *motor);
void VESC_RPM_mode_I(u8 id);
void VESC_position_mode_rpm(u8 id);
void VESC_position_mode_pos(u8 id);
void VESC_position_mode_I(u8 id);
void VESC_Set_Duty_Cycle(u8 controller_ID, float duty_cycle, u8 InConGrpFlag);
void VESC_Set_Speed(u8 controller_ID, s32 speed, u8 InConGrpFlag);
void VESC_Set_Pos(u8 controller_ID, float pos, u8 InConGrpFlag);
void VESC_Set_Current(u8 controller_ID, float current, u8 InConGrpFlag);
void VESC_Set_Brake_Current(u8 controller_ID, float brake_current, u8 InConGrpFlag);
void VESC_Set_Handbrake_Current(u8 controller_ID, float handbrake_current, u8 InConGrpFlag);

#endif
