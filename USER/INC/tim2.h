#ifndef _TIM2_H
#define _TIM2_H

#include "epos.h"
#include "elmo.h"
#include "sys.h"
#include "param.h"
#include "vesc.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
void TIM2_Configuration(void);
void ElmoAction(void);
void ElmoAskStatus(void);

#endif
