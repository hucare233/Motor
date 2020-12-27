#ifndef _COMMON_CAN_H
#define _COMMON_CAN_H

#include "canctrllist.h"
#include "queue.h"

#define CAN_EMERGENCY1  0x81
#define CAN_EMERGENCY2  0x82
#define CAN_EMERGENCY3  0x83
#define CAN_EMERGENCY4  0x84
#define CAN_EMERGENCY5  0x85
#define CAN_EMERGENCY6  0x86
#define CAN_EMERGENCY7  0x87
#define CAN_EMERGENCY8  0x88

void NMT_Operational(CAN_TypeDef* CANx);
void NMT_PreOperational(CAN_TypeDef* CANx);
void NMT_Stopped(CAN_TypeDef* CANx);
void NMT_ResetApplication(CAN_TypeDef* CANx);
void NMT_Communication(CAN_TypeDef* CANx);

#endif
