/*
 * @Descripttion: 电机报文
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2021-01-13 10:02:00
 * @FilePath: \MotoPro\USER\SRC\can2.c
 */
#include "can2.h"
#include "delay.h"
#include "motor.h"
#include "elmo.h"
#include "queue.h"
static u8 Registration_Std_Elmo[2][10] = {{'B', 'S', 'U', 'M', 'S', 'P', 'P', 'B', 'M', 'M'},
										  {'G', 'T', 'M', 'O', 'P', 'X', 'A', 'G', 'O', 'O'}};
static u8 Registration_Std_Epos[2][10] = {{0X40, 0X64, 0X60, 0X40, 0XFF, 0X81, 0X7A, 0X40, 0X40, 0X40},
										  {0X60, 0X60, 0X60, 0X60, 0X60, 0X60, 0X60, 0X60, 0X60, 0X60}};
u32 timeout_ticks = 2000; //200ms
s16 timeout_counts = 0;	  //超时计数
u32 last_update_time[8] = {0};
u32 now_update_time[8] = {0};
u32 err_update_time[8] = {0};
bool clear_flag[8] = {0};
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024; tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为42M,如果设置CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//则波特率为:42M/((6+7+1)*6)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;

u8 CAN2_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);

	/* Configure CAN pin: RX  TX*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* CAN RX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE; //非时间触发通道模式
	CAN_InitStructure.CAN_ABOM = DISABLE; //软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
	CAN_InitStructure.CAN_AWUM = DISABLE; //睡眠模式由软件唤醒
	CAN_InitStructure.CAN_NART = DISABLE; //禁止报文自动发送，即只发送一次，无论结果如何
	CAN_InitStructure.CAN_RFLM = DISABLE; //报文不锁定，新的覆盖旧的
	CAN_InitStructure.CAN_TXFP = DISABLE; //发送FIFO的优先级由标识符决定
	CAN_InitStructure.CAN_Mode = mode;	  //CAN硬件工作在正常模式

	/* Seting BaudRate */
	CAN_InitStructure.CAN_SJW = tsjw;	 //重新同步跳跃宽度为一个时间单位
	CAN_InitStructure.CAN_BS1 = tbs1;	 //时间段1占用8个时间单位
	CAN_InitStructure.CAN_BS2 = tbs2;	 //时间段2占用7个时间单位
	CAN_InitStructure.CAN_Prescaler = 3; //分频系数（Fdiv）
	CAN_Init(CAN2, &CAN_InitStructure);	 //初始化CAN1

	/* 波特率计算公式: BaudRate = APB1时钟频率/Fdiv/（SJW+BS1+BS2） */
	/* 42MHz/3/(1+9+4)=1Mhz */

	//配置过滤器
	//DJ
	CAN_FilterInitStructure.CAN_FilterNumber = 14;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X201 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X202 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X203 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X204 << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 15;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0X205 << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0X206 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X207 << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X208 << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	//elmo
	CAN_FilterInitStructure.CAN_FilterNumber = 16;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = Elmo_Motor1_RX << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = Elmo_Motor2_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = Elmo_Motor3_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = Elmo_Motor4_RX << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 17;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = Elmo_Motor5_RX << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = Elmo_Motor6_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = Elmo_Motor7_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = Elmo_Motor8_RX << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 18;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = Elmo_Motor1_error << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = Elmo_Motor2_error << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = Elmo_Motor3_error << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = Elmo_Motor4_error << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 19;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = Elmo_Motor5_error << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = Elmo_Motor6_error << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = Elmo_Motor7_error << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = Elmo_Motor8_error << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	//VESC
	CAN_FilterInitStructure.CAN_FilterNumber = 20;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((uint32_t)CAN_PACKET_STATUS << 8) << 3) & 0xffff0000) >> 16;
	CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)CAN_PACKET_STATUS << 8) << 3) & 0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0xffffff00 << 3) >> 16;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (0xffffff00 << 3) & 0xffff;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	//EPOS
	CAN_FilterInitStructure.CAN_FilterNumber = 21; //EPOS反馈报文
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = EPOS_Motor1_RX << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = EPOS_Motor2_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = EPOS_Motor3_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = EPOS_Motor4_RX << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 22; //EPOS错误报文
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = EPOS_Motor1_error << 6;
	CAN_FilterInitStructure.CAN_FilterIdLow = EPOS_Motor2_error << 6;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = EPOS_Motor2_error << 6;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = EPOS_Motor2_error << 6;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);

	return 0;
}

static CanTxMsg DJ_tx_message;
/****DJ电机电流输入****/
void currentInput(u8 id)
{
	PEAK(motor[id].valueSet.current, motor[id].intrinsic.CURRENT_LIMIT);
	if (!motor[id].enable)
		motor[id].valueSet.current = 0;
	if (id < 4)
		DJ_tx_message.StdId = 0x200;
	else
		DJ_tx_message.StdId = 0x1FF;
	DJ_tx_message.RTR = CAN_RTR_Data;
	DJ_tx_message.IDE = CAN_Id_Standard;
	DJ_tx_message.DLC = 8;
	u8 temp = 2 * (id & 0x0B);
	EncodeS16Data(&motor[id].valueSet.current, &DJ_tx_message.Data[temp]);
	ChangeData(&DJ_tx_message.Data[temp], &DJ_tx_message.Data[temp + 1]);
	if ((id == 3) || (id == 7))
		CAN_Transmit(CAN2, &DJ_tx_message);
}

//中断服务函数
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;

	CAN_Receive(CAN2, 0, &RxMessage);
	if (
		(RxMessage.IDE == CAN_Id_Standard)												   //标准帧、
		&& (RxMessage.IDE == CAN_RTR_Data)												   //数据帧、
		&& (RxMessage.DLC == 8) && (RxMessage.StdId > 0x200) && (RxMessage.StdId < 0x209)) /* 数据长度为8   DJ电机*/
	{
		u8 id = RxMessage.StdId - 0x201;
		last_update_time[id] = now_update_time[id];
		now_update_time[id] = OSTimeGet();
		err_update_time[id] = now_update_time[id] - last_update_time[id];
		motor[id].valueReal.speed = (RxMessage.Data[2] << 8) | (RxMessage.Data[3]);
		motor[id].valueReal.pulseRead = (RxMessage.Data[0] << 8) | (RxMessage.Data[1]);
		motor[id].valueReal.current = (RxMessage.Data[4] << 8) | (RxMessage.Data[5]);
		motor[id].valueReal.tempeture = RxMessage.Data[6];
		//TODO:将计算放在中断，减小位置丢失
		pulse_caculate(id);
		if (!motor[id].status.clearFlag) //上电第一次进中断清除位置计算误差。
		{
			motor[id].status.clearFlag = true;
			motor[id].argum.distance = 0;
			motor[id].valueReal.pulse = 0;
		}
	}

	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	CAN_ClearFlag(CAN2, CAN_IT_FMP0);
}

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)
//len:数据长度(最大为8)
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CAN2_Send_Msg(u8 *msg, u8 len)
{
	u8 mbox;
	u16 i = 0;
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x12; // 标准标识符为0
	TxMessage.ExtId = 0x12; // 设置扩展标示符（29位）
	TxMessage.IDE = 0;		// 使用扩展标识符
	TxMessage.RTR = 0;		// 消息类型为数据帧，一帧8位
	TxMessage.DLC = len;	// 发送两帧信息
	for (i = 0; i < len; i++)
		TxMessage.Data[i] = msg[i]; // 第一帧信息
	mbox = CAN_Transmit(CAN2, &TxMessage);
	i = 0;
	while ((CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
		i++; //等待发送结束
	if (i >= 0XFFF)
		return 1;
	return 0;
}

void CAN2_RX1_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP1) != RESET)
	{
		CanRxMsg rx_message;
		CAN_Receive(CAN2, CAN_FIFO1, &rx_message);
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
		CAN_ClearFlag(CAN2, CAN_IT_FMP1);
#ifdef USE_EPOS
		if (((rx_message.StdId >= 0x581) && (rx_message.StdId <= 0x585)) && (rx_message.RTR == CAN_RTR_Data)) //EPOS报文
		{
			u8 id = rx_message.StdId - 0x581;
			EPOSmotor[id].argum.timeout = 0;
			if ((rx_message.Data[0] == 0x4F && rx_message.Data[1] == 0x60 && rx_message.Data[2] == 0x60) |
				(rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x62 && rx_message.Data[2] == 0x60) |
				(rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x64 && rx_message.Data[2] == 0x60) |
				(rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x6B && rx_message.Data[2] == 0x60) |
				(rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x6C && rx_message.Data[2] == 0x60) |
				(rx_message.Data[0] == 0x4B && rx_message.Data[1] == 0x40 && rx_message.Data[2] == 0x60) |
				(rx_message.Data[0] == 0x4B && rx_message.Data[1] == 0x77 && rx_message.Data[2] == 0x60))

				if (rx_message.Data[0] == 0x4B && rx_message.Data[1] == 0x40 && rx_message.Data[2] == 0x60) //使能
					EPOSmotor[id].enable = rx_message.Data[4] >> 3;
			if (rx_message.Data[0] == 0x4F && rx_message.Data[1] == 0x60 && rx_message.Data[2] == 0x60) //模式
				EPOSmotor[id].mode = rx_message.Data[4];
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x62 && rx_message.Data[2] == 0x60) //设定位置
			{
				DecodeS32Data(&EPOSmotor[id].valSet.pulse, &rx_message.Data[4]);
				EPOSmotor[id].valSet.angle = EPOSmotor[id].valSet.pulse * (360.f / EPOSmotor[id].intrinsic.RATIO) / 4.0f / EPOSmotor[id].intrinsic.PULSE;
			}
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x64 && rx_message.Data[2] == 0x60) //实际位置
			{
				DecodeS32Data(&EPOSmotor[id].valReal.pulse, &rx_message.Data[4]);
				EPOSmotor[id].valReal.angle = EPOSmotor[id].valReal.pulse * (360.f / EPOSmotor[id].intrinsic.RATIO) / 4.0f / EPOSmotor[id].intrinsic.PULSE;
			}
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x6B && rx_message.Data[2] == 0x60) //设定速度
				DecodeS32Data(&EPOSmotor[id].valSet.speed, &rx_message.Data[4]);
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x6C && rx_message.Data[2] == 0x60) //实际速度
				DecodeS32Data(&EPOSmotor[id].valReal.speed, &rx_message.Data[4]);
			if (rx_message.Data[0] == 0x4B && rx_message.Data[1] == 0x77 && rx_message.Data[2] == 0x60) //实际扭矩电流
			{
				DecodeS32Data(&EPOSmotor[id].valReal.torque, &rx_message.Data[4]);
				//TODO:136是F90扭矩常数
				EPOSmotor[id].valReal.current = EPOSmotor[id].valReal.torque / 136.0f;
			}
			for (int m = 0; m < 10; m++)
			{
				if (Registration_Std_Epos[0][m] == rx_message.Data[1])
				{
					if (Registration_Std_Epos[1][m] == rx_message.Data[2])
					{
						Can2_MesgSentList[id + 4].ReceiveNumber += 1;
						Can2_MesgSentList[id + 4].TimeOut = 0;
						Can2_MesgSentList[id + 4].SendSem--;
						if (Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].InConGrpFlag == true && Can2_Sendqueue.Rear != Can2_Sendqueue.Front)
						{
							if ((Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].ID & 0xF) == id + 1 && Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].Data[0] == rx_message.Data[0] && Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].Data[1] == rx_message.Data[1])
								Can2_Sendqueue.Front = (Can2_Sendqueue.Front + 1) % CAN_QUEUESIZE;
						}
						break;
					}
				}
			}
		}

#endif
#ifdef USE_ELMO
		if (((rx_message.StdId >= 0x281) && (rx_message.StdId <= 0x288)) && (rx_message.RTR == CAN_RTR_Data)) //ELMO反馈报文
		{
			u8 id = rx_message.StdId - 0x281;
			ELMOmotor[id].argum.timeout = 0;
			if ((rx_message.Data[0] == 'A' && rx_message.Data[1] == 'C' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'B' && rx_message.Data[1] == 'G' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'J' && rx_message.Data[1] == 'V' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'M' && rx_message.Data[1] == 'O' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'P' && rx_message.Data[1] == 'R' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'S' && rx_message.Data[1] == 'D' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'S' && rx_message.Data[1] == 'T' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'D' && rx_message.Data[1] == 'C' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'U' && rx_message.Data[1] == 'M' && (rx_message.Data[3] & BIT6) != 1))

				if (rx_message.Data[0] == 'M' && rx_message.Data[1] == 'O' && (rx_message.Data[3] & BIT6) != 1)
				{
					ELMOmotor[id].enable = rx_message.Data[4];
				}
			if (rx_message.Data[0] == 'V' && rx_message.Data[1] == 'X' && (rx_message.Data[3] & BIT6) != 1)
			{
				DecodeS32Data(&ELMOmotor[id].valReal.speed, &rx_message.Data[4]);
				ELMOmotor[id].valReal.speed /= ELMOmotor[id].intrinsic.PULSE / 60;
			}
			if (rx_message.Data[0] == 'P' && rx_message.Data[1] == 'X' && (rx_message.Data[3] & BIT6) != 1)
			{
				s32 pulse;
				DecodeS32Data(&pulse, &rx_message.Data[4]);
				ELMOmotor[id].valReal.pulse = pulse;
				ELMOmotor[id].valReal.angle = pulse * (360.f / ELMOmotor[id].intrinsic.RATIO) / ELMOmotor[id].intrinsic.PULSE / 4;
			}
			if (rx_message.Data[0] == 'I' && rx_message.Data[1] == 'Q' && (rx_message.Data[3] & BIT6) != 1)
			{
				DecodeS32Data(&ELMOmotor[id].valReal.current, &rx_message.Data[4]);
			}
			if (rx_message.Data[0] == 'U' && rx_message.Data[1] == 'M' && (rx_message.Data[3] & BIT6) != 1)
			{
				DecodeS32Data(&ELMOmotor[id].mode, &rx_message.Data[4]);
			}
			if (rx_message.Data[0] == 'A' && rx_message.Data[1] == 'C' && (rx_message.Data[3] & BIT6) != 1)
			{
				DecodeS32Data(&ELMOmotor[id].valReal.ac, &rx_message.Data[4]);
			}
			if (rx_message.Data[0] == 'D' && rx_message.Data[1] == 'C' && (rx_message.Data[3] & BIT6) != 1)
			{
				DecodeS32Data(&ELMOmotor[id].valReal.dc, &rx_message.Data[4]);
			}
			for (int m = 0; m < 10; m++)
			{
				if (Registration_Std_Elmo[0][m] == rx_message.Data[0])
				{
					if (Registration_Std_Elmo[1][m] == rx_message.Data[1])
					{
						Can2_MesgSentList[id].ReceiveNumber += 1;
						Can2_MesgSentList[id].TimeOut = 0;
						Can2_MesgSentList[id].SendSem--;
						if (Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].InConGrpFlag == true && Can2_Sendqueue.Rear != Can2_Sendqueue.Front)
						{
							if ((Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].ID & 0xF) == id + 1 && Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].Data[0] == rx_message.Data[0] && Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].Data[1] == rx_message.Data[1])
								Can2_Sendqueue.Front = (Can2_Sendqueue.Front + 1) % CAN_QUEUESIZE;
						}
						break;
					}
				}
			}
		}

		if (((rx_message.StdId >= 0x81) && (rx_message.StdId <= 0x88)) && (rx_message.RTR == CAN_RTR_Data) && ((rx_message.Data[1] != 0X82) && (rx_message.Data[1] != 0X10))) //ELMO错误报文  TODO:除去8210的错误，不影响正常使用
		{
			u8 id = rx_message.StdId - 0x81;
			insertError(Eerror.head, ELMOERROR | ((id + 1) << 4) | EMERGENCY);
			Motor_Emer_Code = (0x9 << 24) | (rx_message.StdId << 16) | (rx_message.Data[0] << 8) | rx_message.Data[1];
		}

#endif
#ifdef USE_VESC
		if ((rx_message.IDE == CAN_ID_EXT) && (rx_message.RTR == CAN_RTR_Data)) //VESC报文
		{
			int32_t ind = 0;
			u8 id = rx_message.ExtId & 0xff - 1;
			if ((rx_message.ExtId >> 8) == CAN_PACKET_STATUS)
			{
				VESCmotor[id].valReal.speed = get_s32_from_buffer(rx_message.Data, &ind) / VESCmotor[id].instrinsic.POLE_PAIRS;
				VESCmotor[id].valReal.current = buffer_16_to_float(rx_message.Data, 1e1, &ind);
				VESCmotor[id].valReal.angle = buffer_16_to_float(rx_message.Data, 1e1, &ind);
				//位置计算
				ChangeData(&rx_message.Data[6], &rx_message.Data[7]);
				DecodeU16Data(&VESCmotor[id].argum.angleNow, &rx_message.Data[6]);
				VESCmotor[id].argum.distance = VESCmotor[id].argum.angleNow - VESCmotor[id].argum.anglePrv;
				if (VESCmotor[id].argum.fistPos) //TODO:清除起始误差
				{
					VESCmotor[id].argum.fistPos = false;
					VESCmotor[id].argum.distance = 0;
				}
				VESCmotor[id].argum.anglePrv = VESCmotor[id].argum.angleNow;
				if (ABS(VESCmotor[id].argum.distance) > 1800) //TODO:转角差值大于180°
					VESCmotor[id].argum.distance -= SIG(VESCmotor[id].argum.distance) * 3600;
				VESCmotor[id].valReal.position += VESCmotor[id].argum.distance;
				//位置残差更新
				VESCmotor[id].argum.difPosition = VESCmotor[id].valSet.position - VESCmotor[id].valReal.position;
				//锁点记录
				if (VESCmotor[id].begin)
				{
					VESCmotor[id].argum.lockAngle = VESCmotor[id].valReal.angle;
					VESCmotor[id].argum.lockPosition = VESCmotor[id].valReal.position;
				}
			}
			VESCmotor[id].argum.lastRxTim = OSTimeGet();
		}
#endif
		//		if((Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].InConGrpFlag==true) && (Can2_Sendqueue.Rear!=Can2_Sendqueue.Front))
		//						{
		//							if((Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].Data[0] == rx_message.Data[0])
		//								&&(	Can2_Sendqueue.Can_DataSend[Can2_Sendqueue.Front].Data[1] == rx_message.Data[1]))
		//							Can2_Sendqueue.Front = (Can2_Sendqueue.Front + 1) % CAN_QUEUESIZE;
		//						}
	}
}

u8 CAN2_Receive_Msg(u8 *buf)
{
	u32 i;
	CanRxMsg RxMessage;
	if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0)
		return 0;							  //没有接收到数据,直接退出
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage); //读取数据
	for (i = 0; i < RxMessage.DLC; i++)
		buf[i] = RxMessage.Data[i];
	return RxMessage.DLC;
}

/**
 * @author: 叮咚蛋
 * @brief:  电磁阀控制
 */
void valveCtrl(bool status)
{
	CanTxMsg tx_message;
	tx_message.ExtId = 0x00010400;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.IDE = CAN_Id_Extended;
	tx_message.DLC = 4;
	tx_message.Data[0] = 0x04;
	if (status)
	{
		tx_message.Data[1] = 'S' + 0x40; //开
		tx_message.Data[2] = 'N';
	}
	else
	{
		tx_message.Data[1] = 'E' + 0x40; //关
		tx_message.Data[2] = 'F';
	}
	tx_message.Data[3] = 1;
	CAN_Transmit(CAN2, &tx_message);
}
