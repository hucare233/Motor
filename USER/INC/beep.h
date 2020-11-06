#ifndef _BEEP_H
#define _BEEP_H

#include "sys.h"
#include "delay.h"
#include "includes.h"
#include "stm32f4xx_rcc.h"
#define BEEP_OFF      GPIOA->BSRRH = GPIO_Pin_8

#define BEEP_ON       GPIOA->BSRRL = GPIO_Pin_8

#define BEEP_TOGGLE	   GPIOA->ODR ^= GPIO_Pin_8

void Beep_Configuration(void);
void Beep_Show(u8 num);
void Sound(u16 frq);
void play_Music_1(void);
void play_Music_2(void);
#endif

