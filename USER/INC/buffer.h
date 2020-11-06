#ifndef __BUFFER_H
#define __BUFFER_H

#include "stm32f4xx.h"
#include "macro.h"

void buffer_append_int32(uint8_t* buffer, int32_t number, int32_t *index);
float buffer_16_to_float(const uint8_t *buffer, float scale, int32_t *index);
float buffer_32_to_float(const uint8_t *buffer, float scale, int32_t *index);
void DecodeFloatData_From_4byte(float* f, u8* buff);
void EcodeFloatData_to_4byte(float *f, u8 *buff);
s32 get_s32_from_buffer(const u8 *buffer, s32 *index);
s16 get_s16_from_buffer(const u8 *buffer, s32 *index);

#endif
