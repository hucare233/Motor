#ifndef __TIM4_H
#define __TIM4_H

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "param.h"
#include "stm32f4xx_can.h"
#include "pwm.h"


void TIM4_Init(u16 arr,u16 psc);

#endif
