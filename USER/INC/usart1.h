#ifndef __USART1_H
#define __USART1_H

#include "sys.h"
#include "dma.h"
#include "param.h"
#include "includes.h"
#include "stm32f4xx_usart.h"
void USART1_Configuration(void);
void USART1_Send(INT8U *data,INT8U count);

#endif
