#ifndef _PWM_H_
#define _PWM_H_


#include "stm32f4xx.h"

#include "stm32f4xx_rcc.h"

#include "stm32f4xx_tim.h"

#include "stm32f4xx_gpio.h"

#include "param.h"


void PWM_Init(u16 arr,u16 psc);

void pwm_out(struct Engine_Datadef *Steering_Engine);

#endif
