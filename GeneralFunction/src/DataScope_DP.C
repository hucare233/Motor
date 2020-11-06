/*
 * @Descripttion: 示波器
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-10-30 09:53:40
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-10-31 08:40:55
 * @FilePath: \MotoPro\GeneralFunction\src\DataScope_DP.C
 */

#include "DataScope_DP.h"

unsigned char DataScope_OutPut_Buffer[42] = {0}; //串口发送缓冲区

/**
 * @author: 叮咚蛋
 * @brief: 将单精度浮点数据转成4字节数据并存入指定地址 
 * @param target:目标单精度数据
          buf:待写入数组
          beg:指定从数组第几个元素开始写
 * @return 函数无返回
 */

void Float2Byte(float *target, unsigned char *buf, unsigned char beg)
{
	unsigned char *point;
	point = (unsigned char *)target; //得到float的地址
	buf[beg] = point[0];
	buf[beg + 1] = point[1];
	buf[beg + 2] = point[2];
	buf[beg + 3] = point[3];
}

/**
 * @author: 叮咚蛋
 * @brief: 将待发送通道的单精度浮点数据写入发送缓冲区
 * @param Channel：选择通道（1-10）
 * @return 函数无返回
 */

void DataScope_Get_Channel_Data(float Data, unsigned char Channel)
{
	if ((Channel > 10) || (Channel == 0))
		return; //通道个数大于10或等于0，直接跳出，不执行函数
	else
	{
		switch (Channel)
		{
		case 1:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 1);
			break;
		case 2:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 5);
			break;
		case 3:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 9);
			break;
		case 4:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 13);
			break;
		case 5:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 17);
			break;
		case 6:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 21);
			break;
		case 7:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 25);
			break;
		case 8:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 29);
			break;
		case 9:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 33);
			break;
		case 10:
			Float2Byte(&Data, DataScope_OutPut_Buffer, 37);
			break;
		}
	}
}


/**
 * @author: 叮咚蛋
 * @brief: 生成 DataScopeV1.0 能正确识别的帧格式
 * @param Channel_Number，需要发送的通道个数
 * @return 返回发送缓冲区数据个数
           返回0表示帧格式生成失败
 */

unsigned char DataScope_Data_Generate(unsigned char Channel_Number)
{
	if ((Channel_Number > 10) || (Channel_Number == 0))
	{
		return 0;
	} //通道个数大于10或等于0，直接跳出，不执行函数
	else
	{
		DataScope_OutPut_Buffer[0] = '$'; //帧头

		switch (Channel_Number)
		{
		case 1:
			DataScope_OutPut_Buffer[5] = 5;
			return 6;
			break;
		case 2:
			DataScope_OutPut_Buffer[9] = 9;
			return 10;
			break;
		case 3:
			DataScope_OutPut_Buffer[13] = 13;
			return 14;
			break;
		case 4:
			DataScope_OutPut_Buffer[17] = 17;
			return 18;
			break;
		case 5:
			DataScope_OutPut_Buffer[21] = 21;
			return 22;
			break;
		case 6:
			DataScope_OutPut_Buffer[25] = 25;
			return 26;
			break;
		case 7:
			DataScope_OutPut_Buffer[29] = 29;
			return 30;
			break;
		case 8:
			DataScope_OutPut_Buffer[33] = 33;
			return 34;
			break;
		case 9:
			DataScope_OutPut_Buffer[37] = 37;
			return 38;
			break;
		case 10:
			DataScope_OutPut_Buffer[41] = 41;
			return 42;
			break;
		}
	}
	return 0;
}
