#ifndef _EPOS_H
#define _EPOS_H

#include "param.h"
#include "queue.h"
#include "canctrllist.h"
#include "stm32f4xx.h"

/****EPOS_CAN报文ID****/
#define EPOS_Motor1_TX 0x601
#define EPOS_Motor2_TX 0x602
#define EPOS_Motor3_TX 0x603
#define EPOS_Motor4_TX 0x604
#define EPOS_Motor1_RX 0x581
#define EPOS_Motor2_RX 0x582
#define EPOS_Motor3_RX 0x583
#define EPOS_Motor4_RX 0x584
#define EPOS_Motor1_error 0x81
#define EPOS_Motor2_error 0x82
#define EPOS_Motor3_error 0x83
#define EPOS_Motor4_error 0x84

enum EPOS_MODE
{
  NO,
  PPM,
  N,
  PVM
};
/****ELMO内参****/
typedef struct
{
  u16 PULSE;   //编码器线数
  float RATIO; //减速比
} EPOSParam;

/****EPOS外参****/
typedef struct
{
  vs16 angle;    //轴前角度
  float current; //电流
  vs32 speed;    //速度（rpm, 后面计算时转换为JV值与SP值）
  vs32 pulse;    //累计脉冲
  vs32 torque;   //扭矩
} EPOSValue;

/****电机状态****/
typedef struct
{
  bool arrived; //位置模式下到达指定位置
  bool zero;    //寻零完成
  bool stuck;   //电机堵转
  bool timeout; //can报文反馈超时
} EPOSStatus;

/****EPOS限制保护结构体****/
typedef struct
{
  bool isPosLimitON;      //是否对最大位置进行限制，(轴前角度)
  float maxAngle;         //最大旋转角度
  bool isPosSPLimitOn;    //是否对位置模式进行限速
  u16 posSPlimit;         //位置模式下的速度限制
  bool isRealseWhenStuck; //是否堵转时释放电机
  u16 zeroSP;             //寻零模式下的速度
  u16 zeroCurrent;        //寻零模式下的最大电流
} EPOSLimit;

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
} EPOSArgum;

/****EPOS总结构体****/
typedef struct
{
  u8 mode;                   //电机模式
  bool enable;               //电机使能
  EPOSValue valSet, valReal; //外参设定值，实际值
  EPOSStatus status;         //电机状态
  EPOSLimit limit;           //限制保护
  EPOSArgum argum;           //计算所用间值
  EPOSParam intrinsic;       //电机内参
} EPOS_Motor;

typedef struct
{
  bool um;
  bool start;
  bool bgppm;
  bool bgpvm;
  bool disable;
  bool setpvmspeed;
  bool enable;
  bool halt;
  bool quickstop;
  bool setppmspeed;
  bool setppmposition;
  bool relieve;
  bool test;
} eposflag;
void EPOS_Motor_Init(void);
void EPOS_Init(CAN_TypeDef *CANx);
void EPOS_SetMode(u8 ID, s32 mode, u8 InInConGrpFlag);
void EPOS_StartMotor(u8 ID, u8 InInConGrpFlag);
void EPOS_AGAINMotorPPM(u8 ID, u8 InInConGrpFlag);
void EPOS_RelieveMotor(u8 ID, u8 InInConGrpFlag);
void EPOS_SetPVMspeed(u8 ID, s32 speed, u8 InConGrpFlag);
void EPOS_EnableOperation(u8 ID, u8 InConGrpFlag);
void EPOS_QuickStop(u8 ID, u8 InConGrpFlag);
void EPOS_Halt(u8 ID, u8 InConGrpFlag);
void EPOS_SetPPMspeed(u8 ID, s32 speed, u8 InConGrpFlag);
void EPOS_SetPPMposition(u8 ID, s32 position, u8 InConGrpFlag);
void EPOS_Set_HHMoff(u8 ID, s32 position, u8 InConGrpFlag);
void EPOS_Set_HHMmethod(u8 ID, u8 method, u8 InConGrpFlag);
void EPOS_BeginHHM(u8 ID, u8 InConGrpFlag);
void EPOS_SetHHMpos(u8 ID, s32 position, u8 InConGrpFlag);
void EPOS_CONTROL(u8 id);
void EPOS_Askactualspeed(u8 ID, u8 InConGrpFlag);
void EPOS_Askdemandspeed(u8 ID, u8 InConGrpFlag);
void EPOS_Askactualpos(u8 ID, u8 InConGrpFlag);
void EPOS_Askdemandpos(u8 ID, u8 InConGrpFlag);
void EPOS_Askmode(u8 ID, u8 InConGrpFlag);
void EPOS_Askenable_or_disable(u8 ID, u8 InConGrpFlag);
void EPOS_Asktorque(u8 ID, u8 InConGrpFlag);
void EPOS_ReadStatusword(u8 ID, u8 InConGrpFlag);
void EPOS_BootUP(u8 ID, u8 InConGrpFlag);
extern bool Abs;
extern bool Rel;
extern s32 PVMspeed;
extern s32 PPMposition;
extern s32 PPMspeed;
extern s32 Mode;
extern eposflag EPOSflag;
extern EPOSParam FLat90;
extern EPOSLimit EPOSlimit;
extern EPOSArgum EPOSargum;
extern EPOS_Motor EPOSmotor[5];
#endif
