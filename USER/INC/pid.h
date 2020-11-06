#ifndef __PID_H
#define __PID_H

#include "stm32f4xx.h"

/****PID参数****/
typedef struct
{
  volatile s32 SetVal;     //设定值
  volatile float uKP_Coe;  //比例系数
  volatile float uKI_Coe;  //积分常数
  volatile float uKD_Coe;  //微分常数
  volatile float uKS_Coe;  //速度降系
  volatile s32 Udlt;       //pid增量值
  volatile s32 CurVal;     //实际值
  volatile s32 liEkVal[3]; //差值保存，给定和反馈的差值
} PID_setTypeDef;

void PID_Init(PID_setTypeDef *PID, float KP, float KI, float KD, float KS, s32 Set);
void PID_Operation(PID_setTypeDef *PID);
void SetM3508_1(short ID1, short ID2, short ID3, short ID4);
void SetM3508_2(short ID5, short ID6, short ID7, short ID8);
void peakcurrent(void);
#endif
