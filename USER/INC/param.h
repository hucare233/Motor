#ifndef _PARAM_H_

#define _PARAM_H_

#include "stm32f4xx.h"

/**
  *@brief 程序功能选择
	*/
	
#define board_test 0//舵机板测试

/**
  *@brief 板子ID
	*/
#define ID_Self 0x00010400

#define ID_Back 0x00040101

#define broadcast_ID 0x00010000

/**
  *@brief IO口宏定义
	*/
#define PB3 3

#define PB4 4

/**舵机参数结构体**/
struct Engine_Datadef
{
	u8 port;
	
	volatile u8 Start_Flag;
	
	float Start_Ratio;
	
	volatile u8 Move_Flag;
	
	float Move_Ratio;
	
	volatile u8 End_Flag;
	
	float End_Ratio;
};

/** 舵机临时参数**/
typedef struct
{
	u16 Engine_PWM_arr;//pwm比较值（最大可以是899），通过调节这个来调节pwm占空比
	
	u16 Engine_PWM_psc;
	
 }Temp_Datadef;

void param_Init(void);

extern Temp_Datadef temp;
 
extern struct Engine_Datadef Steering_Engine[4];

#endif
