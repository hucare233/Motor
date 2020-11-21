#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm32f4xx.h"
#include "pid.h"
#include "stdbool.h"
#include "can2.h"
#include "macro.h"
#include "led.h"
#include "beep.h"
#include "led8.h"

#define ENABLE_ALL_DJMOTOR_1_4 \
    {                          \
        motor[0].enable = 1;   \
        motor[1].enable = 1;   \
        motor[2].enable = 1;   \
        motor[3].enable = 1;   \
    }

#define DISABLE_ALL_DJMOTOR_1_4 \
    {                           \
        motor[0].enable = 0;    \
        motor[1].enable = 0;    \
        motor[2].enable = 0;    \
        motor[3].enable = 0;    \
    }

#define BEGIN_ALL_DJMOTOR_1_4 \
    {                         \
        motor[0].begin = 1;   \
        motor[1].begin = 1;   \
        motor[2].begin = 1;   \
        motor[3].begin = 1;   \
    }

#define STOP_ALL_DJMOTOR_1_4 \
    {                        \
        motor[0].begin = 0;  \
        motor[1].begin = 0;  \
        motor[2].begin = 0;  \
        motor[3].begin = 0;  \
    }

#define ENABLE_ALL_DJMOTOR_5_8 \
    {                          \
        motor[4].enable = 1;   \
        motor[5].enable = 1;   \
        motor[6].enable = 1;   \
        motor[7].enable = 1;   \
    }

#define DISABLE_ALL_DJMOTOR_5_8 \
    {                           \
        motor[4].enable = 0;    \
        motor[5].enable = 0;    \
        motor[6].enable = 0;    \
        motor[7].enable = 0;    \
    }

#define BEGIN_ALL_DJMOTOR_5_8 \
    {                         \
        motor[4].begin = 1;   \
        motor[5].begin = 1;   \
        motor[6].begin = 1;   \
        motor[7].begin = 1;   \
    }

#define STOP_ALL_DJMOTOR_5_8 \
    {                        \
        motor[4].begin = 0;  \
        motor[5].begin = 0;  \
        motor[6].begin = 0;  \
        motor[7].begin = 0;  \
    }

#define DJ_clear_error                  \
    {                                   \
        motor[0].argum.timeoutCnt = 0; \
        motor[1].argum.timeoutCnt = 0; \
        motor[2].argum.timeoutCnt = 0; \
        motor[3].argum.timeoutCnt = 0; \
        motor[4].argum.timeoutCnt = 0; \
        motor[5].argum.timeoutCnt = 0; \
        motor[6].argum.timeoutCnt = 0; \
        motor[7].argum.timeoutCnt = 0; \
        sprintf(Motor_error,"%s","(。_。)");\
    }
/****电机模式****/
enum MOTOR_MODE
{
    duty = 0,
    current,
    RPM,
    brake,
    zero,
    position,
    handbrake
};

/****电机限制保护结构体****/
typedef struct
{
    bool isPosLimitON;      //是否对最大位置进行限制，(轴前角度)
    float maxAngle;         //最大旋转角度
    bool isPosSPLimitOn;    //是否对位置模式进行限速
    u16 posSPlimit;         //位置模式下的速度限制
    bool isRealseWhenStuck; //是否堵转时释放电机
    s16 zeroSP;             //寻零模式下的速度
    u16 zeroCurrent;        //寻零模式下的最大电流
    bool stuckmotion;
    bool timeoutmotion;
} MotorLimit;

/****电机内参****/
typedef struct
{
    u16 PULSE;         //编码器线数
    u8 RATIO;          //减速比
    u16 CURRENT_LIMIT; //最大输出电流限制
    float GearRatio;
} MotorParam;

/****电机外参****/
typedef struct
{
    vs32 angle;     //轴前角度
    vs16 current;   //电流
    vs16 speed;     //速度 (rpm)
    vs32 pulse;     //累计脉冲
    vs32 pulseRead; //反馈脉冲
    vs16 tempeture; //当前温度
} MotorValue;

/****电机状态****/
typedef struct
{
    bool arrived;   //位置模式下到达指定位置
    bool zero;      //寻零完成
    bool stuck;     //电机堵转
    bool timeout;   //can报文反馈超时
    bool overspeed; //超速
    bool isSetZero; //是否设置当前位置为零点
    bool clearFlag; //清除误差
} MotorStatus;

/****不需要关心的一些参数****/
typedef struct
{
    u32 maxPulse;     //最大旋转脉冲,[相关于最大旋转角度]
    vs32 lockPulse;   //锁位置用当前位置记录，[相关于begin]
    s16 distance;     //当前反馈脉冲与前次反馈脉冲差值，[相关于累计脉冲pulse]
    s32 difPulseSet;  //当前累计脉冲与设定脉冲差值
    u16 zeroCnt;      //寻零用计数值，[相关于寻零状态zero]
    vu16 stuckCnt;    //堵转用计数值，[相关于堵转状态stuck]
    u8 timeoutCnt;    //超时用计数值，[相关于超时状态timeout]
    u32 lastRxTim;    //上次接收到报文的系统时间，[相关于超时状态timeout]
    u16 timeoutTicks; //判断超时用系统节拍数，[相关于超时状态timeout]
} MotorArgum;

/****DJ电机总结构体****/
typedef struct
{
    bool enable;                              //电机使能
    bool begin;                               //电机启动
    u8 mode;                                  //电机模式
    MotorValue valueSet, valueReal, valuePrv; //外参设定值，实际值，前次值
    MotorStatus status;                       //电机状态
    PID_setTypeDef PIDx, PIDs;                //PID参数
    MotorLimit limit;                         //限制保护
    MotorArgum argum;                         //计算所用间值
    MotorParam intrinsic;                     //电机内参
} DJmotor;

typedef struct
{
    bool um;
    bool enable;
    bool begin;
    bool angle;
    bool speed;
} Djflag;

extern bool enable_or_dis;
extern u32 angle;
extern u32 Speed;
extern bool ifbegin;
extern Djflag DJflag;
extern char Motor_error[32];

extern DJmotor motor[8];
void Motor_Init(void);
void speed_mode(s16 id);
void position_mode(s16 id);
void zero_mode(s16 id);
void pulse_caculate(void);
u8 ifstuck(u16 id);
void iftimeout(u16 id);
void djcontrol(void);
void legcontrol(vs16 angle);
#endif
