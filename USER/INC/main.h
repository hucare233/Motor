#ifndef _MAIN_H
#define _MAIN_H

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "can2.h"
#include "can1.h"
#include "pid.h"
#include "tim3.h"
#include "tim2.h"
#include "motor.h"
#include "beep.h"
#include "led8.h"
#include "key.h"
#include "elmo.h"
#include "stm32f4xx.h"
#include "includes.h"
#include "epos.h"
#include "vesc.h"
#include "usart2.h"
#include "pstwo.h"
#include "music.h"
#include "visual_scope.h"
#include "DataScope_DP.h"
/****UCOSII任务设置****/
//START任务
#define START_TASK_PRIO 5
#define START_STK_SIZE 256
__align(8) OS_STK START_TASK_STK[START_STK_SIZE];
static void Task_Start(void *pdata);

//LCD任务
#define LCD_TASK_PRIO 13
#define LCD_STK_SIZE 512
__align(8) OS_STK LCD_TASK_STK[LCD_STK_SIZE];
static void Task_Lcd(void *pdata);

//led8任务
#define LED8_TASK_PRIO 30
#define LED8_STK_SIZE 128
__align(8) OS_STK LED8_TASK_STK[LED8_STK_SIZE];
static void Task_Led8(void *pdata);

//DJ任务
#define MOTOR_TASK_PRIO 9
#define MOTOR_STK_SIZE 256
__align(8) OS_STK MOTOR_TASK_STK[MOTOR_STK_SIZE];
static void Task_Motor(void *pdata);

//ELMO任务
#define ELMO_TASK_PRIO 10
#define ELMO_STK_SIZE 256
__align(8) OS_STK ELMO_TASK_STK[ELMO_STK_SIZE];
static void Task_Elmo(void *pdata);

//epos任务
#define EPOS_TASK_PRIO 15
#define EPOS_STK_SIZE 256
__align(8) OS_STK EPOS_TASK_STK[EPOS_STK_SIZE];
static void Task_EPOS(void *pdata);

//vesc任务
#define VESC_TASK_PRIO 16
#define VESC_STK_SIZE 256
__align(8) OS_STK VESC_TASK_STK[VESC_STK_SIZE];
static void Task_VESC(void *pdata);

//虚拟示波器
#define SCOPE_TASK_PRIO 17
#define SCOPE_STK_SIZE 128
__align(8) OS_STK SCOPE_TASK_STK[SCOPE_STK_SIZE];
static void Task_Scope(void *pdata);

#define DataSCOPE_TASK_PRIO 18
#define DataSCOPE_STK_SIZE 128
__align(8) OS_STK DataSCOPE_TASK_STK[DataSCOPE_STK_SIZE];
static void Task_DataScope(void *pdata);

#define VESCSEND_TASK_PRIO 31
#define VESCSEND_STK_SIZE 128
__align(8) OS_STK VESCSEND_TASK_STK[VESCSEND_STK_SIZE];
static void Task_VESCSEND(void *pdata);

#endif
