/*
 * @Descripttion: 数组转换
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-06 20:34:32
 * @FilePath: \MotoPro\USER\SRC\buffer.c
 */

#include "buffer.h"

/**
	*@brief:将float类型的数据放到四个字节中
	*/
void EcodeFloatData_to_4byte(float *f, u8 *buff)
{
	u16 f_c[2];
	*(float *)f_c = *f;
	buff[0] = (f_c[0] >> 0 & 0x00ff);
	buff[1] = ((f_c[0] >> 8) & 0x00ff);
	buff[2] = (f_c[1] >> 0 & 0x00ff);
	buff[3] = ((f_c[1] >> 8) & 0x00ff);
}
/**
	*@brief:四个字节的数据转化为float类型
	*/
void DecodeFloatData_From_4byte(float *f, u8 *buff)
{
	short int f_c[2];
	f_c[0] = *(short int *)buff;
	f_c[1] = *(short int *)(buff + 2);
	*f = *(float *)f_c;
}
/**
	*@brief:将number放到数组buffer中，number的高位对应buffer的低字节
	*/
void buffer_append_int32(uint8_t *buffer, int32_t number, int32_t *index)
{
	buffer[(*index)++] = number >> 24;
	buffer[(*index)++] = number >> 16;
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}
/**
	*@brief:将四个字节u8类型的数据合成为int类型返回
	*/
s32 get_s32_from_buffer(const u8 *buffer, s32 *index)
{
	s32 res = ((uint32_t)buffer[*index]) << 24 |
			  ((uint32_t)buffer[*index + 1]) << 16 |
			  ((uint32_t)buffer[*index + 2]) << 8 |
			  ((uint32_t)buffer[*index + 3]);
	*index += 4;
	return res;
}
/**
  *@brief:将两个字节u8类型的数据合成为int类型返回
  */
s16 get_s16_from_buffer(const u8 *buffer, s32 *index)
{
	s16 res = ((uint32_t)buffer[*index]) << 8 |
			  ((uint32_t)buffer[*index + 1]);
	*index += 2;
	return res;
}
/**
	*@brief:将四个字节u8类型的数据合成为float类型除以scale之后返回
	*/
float buffer_32_to_float(const uint8_t *buffer, float scale, int32_t *index)
{
	return (float)get_s32_from_buffer(buffer, index) / scale;
}
/**
	*@brief:将两个字节u8类型的数据合成为float类型除以scale之后返回
	*/
float buffer_16_to_float(const uint8_t *buffer, float scale, int32_t *index)
{
	return (float)get_s16_from_buffer(buffer, index) / scale;
}
