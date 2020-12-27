/*
 * @Descripttion: 
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-22 10:01:17
 * @FilePath: \MotoPro\USER\INC\can1.h
 */
#ifndef __CAN1_H
#define __CAN1_H

#include "elmo.h"
#include "tim3.h"
#include "param.h"
#include "macro.h"
#include "includes.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

void CAN1_Configuration(void);
void feedbackAngle(void);
extern s16 kl_angle;
#endif
