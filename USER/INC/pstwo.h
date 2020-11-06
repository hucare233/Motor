/*
 * @Descripttion: 
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-07 00:12:47
 * @FilePath: \MotoPro\USER\INC\pstwo.h
 */
#ifndef __PSTWO_H
#define __PSTWO_H
#include "stm32f4xx.h"
#include "delay.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "beep.h"
/*********************************************************
Copyright (C), 2015-2025, YFRobot.
www.yfrobot.com
File：PS2驱动程序
Author：pinggai    Version:1.0     Data:2015/05/16
Description: PS2驱动程序
**********************************************************/	 
//in->pc3  out->pc2  cs->pc0  clk->pc1

#define DI   GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)           //  输入

#define DO_H GPIO_SetBits(GPIOC, GPIO_Pin_2)        //命令位高
#define DO_L GPIO_ResetBits(GPIOC, GPIO_Pin_2)       //命令位低

#define CS_H GPIO_SetBits(GPIOC, GPIO_Pin_0)       //CS拉高
#define CS_L GPIO_ResetBits(GPIOC, GPIO_Pin_0)       //CS拉低

#define CLK_H GPIO_SetBits(GPIOC, GPIO_Pin_1)      //时钟拉高
#define CLK_L GPIO_ResetBits(GPIOC, GPIO_Pin_1)      //时钟拉低


//These are our button constants

#define PS2_SELECT      (!key3_buf[0])
#define PS2_L3          (!key3_buf[1])
#define PS2_R3          (!key3_buf[2])
#define PS2_START       (!key3_buf[3])
#define PS2_PAD_UP      (!key3_buf[4])
#define PS2_PAD_RIGHT   (!key3_buf[5])
#define PS2_PAD_DOWN    (!key3_buf[6])
#define PS2_PAD_LEFT    (!key3_buf[7])
#define PS2_L2          (!key4_buf[0])
#define PS2_R2          (!key4_buf[1])
#define PS2_L1          (!key4_buf[2])
#define PS2_R1          (!key4_buf[3])
#define PS2_GREEN       (!key4_buf[4])
#define PS2_RED         (!key4_buf[5])
#define PS2_BLUE        (!key4_buf[6])
#define PS2_PINK        (!key4_buf[7])
#define PS2_TRIANGLE    13
#define PS2_CIRCLE      14
#define PS2_CROSS       15
#define PS2_SQUARE      26

//#define WHAMMY_BAR		8

//These are stick values
#define PSS_RX 5                //右摇杆X轴数据
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8



extern u8 Data[9];
extern u16 MASK[16];
extern u16 Handkey;

void PS2_Init(void);
void handle_button(void);
unsigned char psx_transfer(unsigned char dat);
void request_ps2(void);
void ps2_step(void);

//u8 PS2_RedLight(void);   //判断是否为红灯模式
//void PS2_ReadData(void); //读手柄数据
//void PS2_Cmd(u8 CMD);		  //向手柄发送命令
//u8 PS2_DataKey(void);		  //按键值读取
//u8 PS2_AnologData(u8 button); //得到一个摇杆的模拟量
//void PS2_ClearData(void);	  //清除数据缓冲区
//void PS2_Vibration(u8 motor1, u8 motor2);//振动设置motor1  0xFF开，其他关，motor2  0x40~0xFF

//void PS2_EnterConfing(void);	 //进入配置
//void PS2_TurnOnAnalogMode(void); //发送模拟量
//void PS2_VibrationMode(void);    //振动设置
//void PS2_ExitConfing(void);	     //完成配置
//void PS2_SetInit(void);		     //配置初始化

#endif

