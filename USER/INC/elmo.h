#ifndef _ELMO_H
#define _ELMO_H

#include "stm32f4xx.h"
#include "stdbool.h"
#include "can2.h"
#include "macro.h"

#define GetData 1
#define SetData 2

/****ELMO CAN报文ID****/
#define Elmo_Motor1_RX 0x281
#define Elmo_Motor2_RX 0x282
#define Elmo_Motor3_RX 0x283
#define Elmo_Motor4_RX 0x284
#define Elmo_Motor5_RX 0x285
#define Elmo_Motor6_RX 0x286
#define Elmo_Motor7_RX 0x287
#define Elmo_Motor8_RX 0x288
#define Elmo_Motor1_TX 0x301
#define Elmo_Motor2_TX 0x302
#define Elmo_Motor3_TX 0x303
#define Elmo_Motor4_TX 0x304
#define Elmo_Motor5_TX 0x305
#define Elmo_Motor6_TX 0x306
#define Elmo_Motor7_TX 0x307
#define Elmo_Motor8_TX 0x308
#define Elmo_Motor1_error 0x81
#define Elmo_Motor2_error 0x82
#define Elmo_Motor3_error 0x83
#define Elmo_Motor4_error 0x84
#define Elmo_Motor5_error 0x85
#define Elmo_Motor6_error 0x86
#define Elmo_Motor7_error 0x87
#define Elmo_Motor8_error 0x88

#define ENABLE_ALL_ELMO                      \
  {                                          \
    for (u8 i = 0; i < 3; i++)               \
    {                                        \
      Elmo_Motor_Enable_Or_Disable(1, 1, 1); \
      Delay_ms(10);                          \
      Elmo_Motor_Enable_Or_Disable(2, 1, 1); \
      Delay_ms(10);                          \
      Elmo_Motor_Enable_Or_Disable(3, 1, 1); \
      Delay_ms(10);                          \
      Elmo_Motor_Enable_Or_Disable(4, 1, 1); \
    }                                        \
  }

#define DISABLE_ALL_ELMO                   \
  {                                        \
    for(u8 i=0;i<3;i++)                    \
		{                                      \
		Elmo_Motor_Enable_Or_Disable(1, 0, 1); \
    Delay_ms(10);                          \
    Elmo_Motor_Enable_Or_Disable(2, 0, 1); \
    Delay_ms(10);                          \
    Elmo_Motor_Enable_Or_Disable(3, 0, 1); \
    Delay_ms(10);                          \
    Elmo_Motor_Enable_Or_Disable(4, 0, 1); \
		}                                      \
  }

#define BEGIN_ALL_ELMO         \
  {                            \
    Elmo_Motor_BG(1, 1);       \
    Elmo_Motor_BG(2, 1);       \
    Elmo_Motor_BG(3, 1);       \
    Elmo_Motor_BG(4, 1);       \
    ELMOmotor[0].begin = true; \
    ELMOmotor[1].begin = true; \
    ELMOmotor[2].begin = true; \
    ELMOmotor[3].begin = true; \
  }

#define STOP_ALL_ELMO          \
  {                            \
    Elmo_Motor_ST(1, 1);       \
    Elmo_Motor_ST(2, 1);       \
    Elmo_Motor_ST(3, 1);       \
    Elmo_Motor_ST(4, 1);       \
    ELMOmotor[0].begin = false; \
    ELMOmotor[1].begin = false; \
    ELMOmotor[2].begin = false; \
    ELMOmotor[3].begin = false; \
  }

/****ELMO内参****/
typedef struct
{
  u16 PULSE;   //编码器线数
  float RATIO; //减速比
} ELMOParam;

/****ELMO外参****/
typedef struct
{
  vs16 angle;   //轴前角度
  vs16 current; //电流
  vs32 speed;   //速度（rpm, 后面计算时转换为JV值与SP值）
  vs32 pulse;   //累计脉冲
} ELMOValue;

/****电机状态****/
typedef struct
{
  bool arrived; //位置模式下到达指定位置
  bool zero;    //寻零完成
  bool stuck;   //电机堵转
  bool timeout; //can报文反馈超时
} ELMOStatus;

/***电机标志位***/
typedef struct
{
  bool bg;
  bool mo;
  bool pa;
  bool sp;
  bool sv;
  bool um;
  bool jv;
  bool st;
  bool px;
  bool mp;
  bool qt;
  bool qv;
  bool rm;
} ELMOflag;
/****ELMO限制保护结构体****/
typedef struct
{
  bool isPosLimitON;      //是否对最大位置进行限制，(轴前角度)
  float maxAngle;         //最大旋转角度
  bool isPosSPLimitOn;    //是否对位置模式进行限速
  u16 posSPlimit;         //位置模式下的速度限制
  bool isRealseWhenStuck; //是否堵转时释放电机
  u16 zeroSP;             //寻零模式下的速度
  u16 zeroCurrent;        //寻零模式下的最大电流
} ELMILimit;

/****不需要关心的一些参数****/
typedef struct
{
  u32 maxPulse;     //最大旋转脉冲,[相关于最大旋转角度]
  u16 zeroCnt;      //寻零用计数值，[相关于寻零状态zero]
  vu16 stuckCnt;    //堵转用计数值，[相关于堵转状态stuck]
  u16 timeoutCnt;   //超时用计数值，[相关于超时状态timeout]
  u32 lastRxTim;    //上次接收到报文的系统时间，[相关于超时状态timeout]
  u16 timeoutTicks; //判断超时用系统节拍数，[相关于超时状态timeout]
  u8 kickCnt;       //踢球次序
  u8 timeout;
  u16 timepre;
  u16 timecut;
  u8 arivecnt;
} ELMOArgum;

/****ELMO总结构体****/
typedef struct
{
  u8 mode; //电机模式
  bool begin;
  bool enable;               //电机使能
  ELMOValue valSet, valReal; //外参设定值，实际值
  ELMOStatus status;         //电机状态
  ELMILimit limit;           //限制保护
  ELMOArgum argum;           //计算所用间值
  ELMOParam intrinsic;       //电机内参
} ELMO_Motor;
extern ELMOflag ELMOFlag;
extern ELMO_Motor ELMOmotor[5];
extern u32 speed;
extern u8 Um;
extern s32 Pa;
extern s32 Jv;
extern s32 Px;
extern s32 Motor_Emer_Code;
void Elmo_Motor_PA(u32 ID, s32 PA, u8 InConGrpFlag);
void Elmo_Motor_PR(u32 ID, s32 PR, u8 InConGrpFlag);
void Elmo_Motor_SP(u32 ID, s32 speed, u8 InConGrpFlag);
void Elmo_Motor_BG(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_Enable_Or_Disable(u32 ID, u8 enable_or_disable, u8 InConGrpFlag);
void Elmo_Motor_SV(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_UM(u32 ID, u8 UM, u8 InConGrpFlag);
void Elmo_Motor_PT(u32 ID, u8 PT, u8 InConGrpFlag);
void Elmo_Motor_PV(u32 ID, u8 PV, u8 InConGrpFlag);
void ELMO_Init(CAN_TypeDef *CANx);
void ELMO_Motor_Init(void);
void elmo_control(u32 id);
void Elmo_Motor_PX(u32 ID, s32 data, u8 InConGrpFlag);
void Elmo_Motor_JV(u32 ID, s32 JV, u8 InConGrpFlag);
void Elmo_Motor_ST(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_RM(u32 ID, s32 data, u8 InConGrpFlag);
void Elmo_Motor_ASKvx(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_ASKmo(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_ASKsp(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_ASKiq(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_ASKum(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_ASKpx(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_ASKpa(u32 ID, u8 InConGrpFlag);
void Elmo_Motor_MP(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag);
void Elmo_Motor_QP(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag);
void Elmo_Motor_QT(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag);
void Elmo_Motor_QV(u32 ID, u8 Sub, s32 data, u8 InConGrpFlag);
#endif
