#ifndef __VESC_H
#define __VESC_H

#include "sys.h"
#include "stdint.h"
#include "param.h"
#include "queue.h"
#include "canctrllist.h"
#include "buffer.h"
#include "motor.h"
/****VESC内参****/
typedef struct
{
	u8 POLE_PAIRS; //电机极对数
} VESCParam;

/****VESC外参****/
typedef struct
{
	volatile float current; //电流
	vs32 speed;				//速度
	vs32 position;			//位置
	volatile float duty;	//占空比
} VESCVal;

/****VESC状态****/
typedef struct
{
	bool stuck;	  //电机堵转
	bool timeout; //can报文反馈超时
} VESCStatus;

/****VESC限制保护****/
typedef struct
{    
	float breakCurrent; //刹车电流
} VESCLimit;

/***不需要关心的一些参数*****/
typedef struct
{
	u8 timeoutCnt;	  //超时用计数值，[相关于超时状态timeout]
	u32 lastRxTim;	  //上次接收到报文的系统时间，[相关于超时状态timeout]
	u16 timeoutTicks; //判断超时用系统节拍数，[相关于超时状态timeout]
	u8 timeout;
} VESCArgum;

/****VESC电机结构体****/
typedef struct
{
	bool enable;			 //电机使能
	bool begin;				 //电机启动
	u8 mode;				 //电机模式
	VESCVal valSet, valReal; //外参设定值，实际值
	VESCStatus status;		 //电机状态
	VESCArgum argum;		 //间值参数
	VESCLimit limit;		 //电机限制保护
	VESCParam instrinsic;	 //电机内参
} VESC_MOTOR;

typedef struct
{
	bool enduty;
	bool enspeed;
	bool enppm;
	bool enhandle;
} VESCflag;
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
extern s32 setduty;
extern s32 setspeed;
extern s32 setosition;
extern s32 handlecurect;
void VESCInit(void);
void VESC_Set_Duty_Cycle(u8 controller_ID, float duty_cycle, u8 InConGrpFlag);
void VESC_Set_Speed(u8 controller_ID, s32 speed, u8 InConGrpFlag);
void VESC_Set_Current(u8 controller_ID, float current, u8 InConGrpFlag);
void VESC_Set_Brake_Current(u8 controller_ID, float brake_current, u8 InConGrpFlag);
void VESC_Set_Handbrake_Current(u8 controller_ID, float handbrake_current, u8 InConGrpFlag);
void VESC_CONTROL(u8 id);

#endif
