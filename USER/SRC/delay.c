/*
 * @Descripttion: 
 * @version: �ڶ���
 * @Author: ���˵�
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-11-06 20:34:43
 * @FilePath: \MotoPro\USER\SRC\delay.c
 */
#include "delay.h"

/** 
  * @brief  ��ʱ������
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
  * @brief  ��ʱ��΢��
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
  * @brief  ��ʱ��ָ��������
  */

void Delay(u16 t)
{
	while (t--)
		;
}
