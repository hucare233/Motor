/*
 * @Descripttion: 手柄
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-11-06 19:26:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-06 20:35:23
 * @FilePath: \MotoPro\USER\SRC\pstwo.c
 */
#include "pstwo.h"
/*********************************************************
Copyright (C), 2015-2025, YFRobot.
www.yfrobot.com
File：PS2驱动程序
Author：pinggai    Version:1.0     Data:2015/05/16
Description: PS2驱动程序
**********************************************************/

extern s8 pauseflag;
extern int walk_num_set, jump_num_set, bound_num_set;
extern float lock_yaw, lock_roll, lock_pitch;
extern u8 beep_on_num;
extern u8 one_num_over;
extern u8 mode_choose;
float max_height = 400.0f;
float min_height = 0.0f;

u16 Handkey;
u8 Comd[2] = {0x01, 0x42};												 //开始命令。请求数据
u8 ps2_data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //数据存储数组

//手柄接口初始化
//in->pc3  out->pc2  cs->pc0  clk->pc1
void PS2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	CS_H;
	DO_H;
	CLK_H;
}
s16 left_y_num;
s16 left_x_num;
s16 right_y_num;
s16 right_x_num;
u8 key3_buf[8] = {1, 1, 1, 1, 1, 1, 1, 1};
u8 key4_buf[8] = {1, 1, 1, 1, 1, 1, 1, 1};

void handle_button(void)
{
	static unsigned char psx_button_bak[2] = {0}; //记录上一次值
	u8 i = 0;
	if ((psx_button_bak[0] == ps2_data[3]) && (psx_button_bak[1] == ps2_data[4]))
	{
	}
	else
	{
		// psx_buf[1] : 0x41 GREEN, 0x73 RED
		//		parse_psx_buf(ps2_data+3, ps2_data[1]);
		psx_button_bak[0] = ps2_data[3];
		psx_button_bak[1] = ps2_data[4];
	}
	if (ps2_data[1] == 0x41) //green_mode
	{
	}
	else if (ps2_data[1] == 0x73) //red_mode
	{
		// Lps2_data[7] 左右摇杆 左0x00 右 0xFF  7f中间
		// Lps2_data[8]前后摇杆 前0x00 后0xFF    80中间
		//ps2_data[5]ps2_data[6]同上
		for (i = 0; i < 8; i++)
		{
			key3_buf[i] = (ps2_data[3] >> i) & 0x01;
			key4_buf[i] = (ps2_data[4] >> i) & 0x01;
		}
		for (i = 5; i < 9; i++)
		{
			if (((ps2_data[i] - 127) < 15) && ((ps2_data[i] - 127) > -15))
			{
				ps2_data[i] = 0x7f;
			}
		}
		//处理按键命令
		if (true == PS2_SELECT)
		{
			Beep_Show(1);
		}
		if (true == PS2_R1)
		{
			Beep_Show(1);
		}
		if (true == PS2_R2)
		{
			Beep_Show(2);
		}
		if (true == PS2_L1)
		{
			Beep_Show(1);
		}
		if (true == PS2_L2)
		{
			Beep_Show(2);
		}
		if (true == PS2_R3)
		{
			Beep_Show(3);
		}
		if (true == PS2_L3)
		{
			Beep_Show(3);
		}
		//			if(True == PS2_PINK)
		//			{
		//				balance_play();
		//				my_flag.balance_play;
		//			}
		if (true == PS2_BLUE)
		{
			Beep_Show(1);
		}
		if (true == PS2_GREEN)
		{
			Beep_Show(1);
		}
		if (true == PS2_RED)
		{
			Beep_Show(1);
		}
		if (true == PS2_START)
		{
			Beep_Show(1);
		}

		// L ps2_data[7] 左右摇杆 左0x00 右 0xFF
		// L ps2_data[8]前后摇杆 前0x00 后0xFF
		// R ps2_data[5]ps2_data[6]同上
		if ((127 - ps2_data[6]) > abs(ps2_data[5] - 127)) //up
		{
		}
		if ((127 - ps2_data[6]) < (ps2_data[5] - 127) && (127 - ps2_data[6]) < -(ps2_data[5] - 127)) //down
		{
		}
		if ((127 - ps2_data[6]) > (ps2_data[5] - 127) && (127 - ps2_data[6]) < -(ps2_data[5] - 127)) //left
		{
		}
		if ((127 - ps2_data[6]) < (ps2_data[5] - 127) && (127 - ps2_data[6]) > -(ps2_data[5] - 127)) //right
		{
		}
		if (true == PS2_PAD_UP) //前
		{
		}
		if (true == PS2_PAD_DOWN) //后
		{
		}
		if (true == PS2_PAD_LEFT) //左
		{
		}
		if (true == PS2_PAD_RIGHT) //右
		{
		}
		//	ps2_step();

		//得到手柄模拟量
		//				lock_yaw += -(ps2_data[5] - 127) * 0.2f;

		//				if(True == my_flag.motor_enable)//只有在使能后才接受模拟量
		//				{
		//					fly_lock.roll_set = -(ps2_data[7] - 127) * 0.2f;
		//					fly_lock.pitch_set = (ps2_data[8] - 127) * 0.2f;
		//					fly_lock.yaw_set = -(ps2_data[5] - 127) * 0.2f;
		//					fly_lock.height_set -= (ps2_data[6] - 127) * 0.002f;
		//				}

		//			if(fly_lock.height_set > max_height)
		//				fly_lock.height_set = max_height;//限高
		//			if(fly_lock.height_set < min_height)
		//				fly_lock.height_set = min_height;
	}
}

unsigned char psx_transfer(unsigned char dat)
{
	unsigned char rd_data, wt_data, i;
	wt_data = dat;
	rd_data = 0;
	for (i = 0; i < 8; i++)
	{
		if ((wt_data & (0x01 << i)))
			DO_H;
		else
			DO_L;

		CLK_H;
		Delay_us(8);
		CLK_L;
		Delay_us(8);
		CLK_H;
		if (DI)
		{
			rd_data |= 0x01 << i;
		}
	}
	return rd_data;
}

//void ps2_step(void)
//{
//	if (false == PS2_PAD_UP && false == PS2_PAD_DOWN && false == PS2_PAD_LEFT && false == PS2_PAD_RIGHT) //无方向按键按下
//	{
//		if (true == my_flag.step_over)
//		{
//			walk_num_set = 0;
//			bound_num_set = 0;
//			jump_num_set = 0;
//			my_flag.ps2_run = False;
//			if (true == my_flag.PrepFinish)
//				my_flag.tick = False; //停止t_tick计时
//		}
//		else if (true == my_flag.ps2_run)
//		{
//			walk_num_set = 1;
//			bound_num_set = 1;
//			jump_num_set = 1;
//			my_flag.tick = true;
//		}
//		else
//		{
//			walk_num_set = 0;
//			jump_num_set = 0;
//			bound_num_set = 0;
//			my_flag.ps2_run = False;
//			if (true == my_flag.PrepFinish) //prep&change的时候不停止tick计时
//				my_flag.tick = False;
//		}
//	}
//	else
//	{
//		walk_num_set = 1;
//		bound_num_set = 1;
//		jump_num_set = 1;
//		my_flag.ps2_run = true;
//		my_flag.tick = true;
//	}
//}
void psx_write_read(unsigned char *get_buf)
{
	CS_L;
	get_buf[0] = psx_transfer(Comd[0]);
	get_buf[1] = psx_transfer(Comd[1]);
	get_buf[2] = psx_transfer(get_buf[0]);
	get_buf[3] = psx_transfer(get_buf[0]);
	get_buf[4] = psx_transfer(get_buf[0]);
	get_buf[5] = psx_transfer(get_buf[0]);
	get_buf[6] = psx_transfer(get_buf[0]);
	get_buf[7] = psx_transfer(get_buf[0]);
	get_buf[8] = psx_transfer(get_buf[0]);
	CS_H;
}

/**
  * @author    WHU_ROBOCON_LJB
  * @date:     2020-07-17
  * @brief     请求ps2手柄命令
  */
void request_ps2(void)
{
	psx_write_read(ps2_data);
}

////向手柄发送命令
//void PS2_Cmd(u8 CMD)
//{
//	volatile u16 ref=0x01;
//	ps2_data[1] = 0;
//	for(ref=0x01;ref<0x0100;ref<<=1)
//	{
//		if(ref&CMD)
//		{
//			DO_H;                   //输出以为控制位
//		}
//		else DO_L;

//		CLK_H;                        //时钟拉高
//		delay_us(50);
//		CLK_L;
//		delay_us(50);
//		CLK_H;
//		if(DI)
//			ps2_data[1] = ref|ps2_data[1];
//	}
//}
////判断是否为红灯模式
////返回值；0，红灯模式
////		  其他，其他模式
//u8 PS2_RedLight(void)
//{
//	CS_L;
//	PS2_Cmd(Comd[0]);  //开始命令
//	PS2_Cmd(Comd[1]);  //请求数据
//	CS_H;
//	if( ps2_data[1] == 0X73)   return 0 ;
//	else return 1;

//}
////读取手柄数据
//void PS2_ReadData(void)
//{
//	volatile u8 byte=0;
//	volatile u16 ref=0x01;

//	CS_L;

//	PS2_Cmd(Comd[0]);  //开始命令
//	PS2_Cmd(Comd[1]);  //请求数据

//	for(byte=2;byte<9;byte++)          //开始接受数据
//	{
//		for(ref=0x01;ref<0x100;ref<<=1)
//		{
//			CLK_H;
//			CLK_L;
//			delay_us(50);
//			CLK_H;
//		      if(DI)
//		      ps2_data[byte] = ref|ps2_data[byte];
//		}
//        delay_us(50);
//	}
//	CS_H;
//}

////对读出来的PS2的数据进行处理      只处理了按键部分         默认数据是红灯模式  只有一个按键按下时
////按下为0， 未按下为1
//u8 PS2_DataKey()
//{
//	u8 index;

//	PS2_ClearData();
//	PS2_ReadData();

//	Handkey=(ps2_data[4]<<8)|ps2_data[3];     //这是16个按键  按下为0， 未按下为1
//	for(index=0;index<16;index++)
//	{
//		if((Handkey&(1<<(MASK[index]-1)))==0)
//		return index+1;
//	}
//	return 0;          //没有任何按键按下
//}

////得到一个摇杆的模拟量	 范围0~256
//u8 PS2_AnologData(u8 button)
//{
//	return ps2_data[button];
//}

////清除数据缓冲区
//void PS2_ClearData()
//{
//	u8 a;
//	for(a=0;a<9;a++)
//		ps2_data[a]=0x00;
//}
//u8 ps2_motor1 = 0x00,ps2_motor2 = 0x40;
///******************************************************
//Function:    void PS2_Vibration(u8 motor1, u8 motor2)
//Description: 手柄震动函数，
//Calls:		 void PS2_Cmd(u8 CMD);
//Input: motor1:右侧小震动电机 0x00关，其他开
//	   motor2:左侧大震动电机 0x40~0xFF 电机开，值越大 震动越大
//******************************************************/
//void PS2_Vibration(u8 _ps2_motor1, u8 _ps2_motor2)
//{
//	CS_L;
//	delay_us(16);
//    PS2_Cmd(0x01);  //开始命令
//	PS2_Cmd(0x42);  //请求数据
//	PS2_Cmd(0X00);
//	PS2_Cmd(_ps2_motor1);
//	PS2_Cmd(_ps2_motor2);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	CS_H;
//	delay_us(16);
//}
////short poll
//void PS2_ShortPoll(void)
//{
//	CS_L;
//	delay_us(16);
//	PS2_Cmd(0x01);
//	PS2_Cmd(0x42);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0x00);
//	PS2_Cmd(0x00);
//	CS_H;
//	delay_us(16);
//}
////进入配置
//void PS2_EnterConfing(void)
//{
//    CS_L;
//	delay_us(16);
//	PS2_Cmd(0x01);
//	PS2_Cmd(0x43);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0x01);
//	PS2_Cmd(0x00);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	CS_H;
//	delay_us(16);
//}
////发送模式设置
//void PS2_TurnOnAnalogMode(void)
//{
//	CS_L;
//	PS2_Cmd(0x01);
//	PS2_Cmd(0x44);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0x01); //analog=0x01;digital=0x00  软件设置发送模式
//	PS2_Cmd(0xEE); //Ox03锁存设置，即不可通过按键“MODE”设置模式。
//				   //0xEE不锁存软件设置，可通过按键“MODE”设置模式。
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0X00);
//	CS_H;
//	delay_us(16);
//}
////振动设置
//void PS2_VibrationMode(void)
//{
//	CS_L;
//	delay_us(16);
//	PS2_Cmd(0x01);
//	PS2_Cmd(0x4D);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0x00);
//	PS2_Cmd(0X01);
//	CS_H;
//	delay_us(16);
//}
////完成并保存配置
//void PS2_ExitConfing(void)
//{
//    CS_L;
//	delay_us(16);
//	PS2_Cmd(0x01);
//	PS2_Cmd(0x43);
//	PS2_Cmd(0X00);
//	PS2_Cmd(0x00);
//	PS2_Cmd(0x5A);
//	PS2_Cmd(0x5A);
//	PS2_Cmd(0x5A);
//	PS2_Cmd(0x5A);
//	PS2_Cmd(0x5A);
//	CS_H;
//	delay_us(16);
//}
////手柄配置初始化
//void PS2_SetInit(void)
//{
//	PS2_ShortPoll();
//	PS2_ShortPoll();
//	PS2_ShortPoll();
//	PS2_EnterConfing();		//进入配置模式
//	PS2_TurnOnAnalogMode();	//“红绿灯”配置模式，并选择是否保存
//	PS2_VibrationMode();	//开启震动模式
//	PS2_ExitConfing();		//完成并保存配置
//}
