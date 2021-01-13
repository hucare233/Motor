#ifndef __PID_H
#define __PID_H

#include "stm32f4xx.h"
#include <math.h>
#include "utils.h"
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

typedef struct
{
  float kp;
  float ki;
  float kd;
  float kd_filter;
  float d_filter;
  float p_term;
  float i_term;
  float d_term;
  s32 error;
  s32 prv_error;
  float output;
  s32 CurVal;
  s32 SetVal;
}VESC_PID_setTypeDef;

void PID_Init(PID_setTypeDef *PID, float KP, float KI, float KD, float KS, s32 Set);
void PID_Operation(PID_setTypeDef *PID);
void SetM3508_1(short ID1, short ID2, short ID3, short ID4);
void SetM3508_2(short ID5, short ID6, short ID7, short ID8);
void peakcurrent(void);
void VESC_PID_Init(VESC_PID_setTypeDef* PID, float KP,float KI,float KD,float KD_Fliter);
void VESC_PID_Operation(VESC_PID_setTypeDef* PID, s32 setval, s32 curval);
#endif
