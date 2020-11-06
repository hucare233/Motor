/*
 * @Descripttion: 
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-06 20:34:43
 * @FilePath: \MotoPro\USER\SRC\delay.c
 */
#include "delay.h"

/** 
  * @brief  延时、毫秒
  */

void Delay_ms(unsigned int t)
{
	int i;
	for (i = 0; i < t; i++)
	{
		int a = 42000;
		while (a--)
			;
	}
}

/** 
  * @brief  延时、微秒
  */
void Delay_us(unsigned int t)
{
	int i;
	for (i = 0; i < t; i++)
	{
		int a = 40;
		while (a--)
			;
	}
}

/** 
  * @brief  延时、指令周期数
  */

void Delay(u16 t)
{
	while (t--)
		;
}
