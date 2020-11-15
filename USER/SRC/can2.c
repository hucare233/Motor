/*
 * @Descripttion: �������
 * @version: �ڶ���
 * @Author: ���˵�
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: ���˵�
 * @LastEditTime: 2020-11-14 16:20:09
 * @FilePath: \MotoPro\USER\SRC\can2.c
 */
#include "can2.h"
#include "delay.h"
#include "motor.h"
#include "elmo.h"
#include "queue.h"

u32 timeout_ticks = 2000; //200ms
s16 timeout_counts = 0;	  //��ʱ����
u32 last_update_time[8] = {0};
u32 now_update_time[8] = {0};
bool clear_flag[8] = {0};
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;

u8 CAN2_Mode_Init(u8 tsjw, u8 tbs2, u8 tbs1, u16 brp, u8 mode)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //ʹ��PORTBʱ��

	//CAN
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	//CAN
	CAN_DeInit(CAN2);

	//��ʼ��GPIO
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM = DISABLE;  //��ʱ�䴥��ͨ��ģʽ
	CAN_InitStructure.CAN_ABOM = DISABLE;  //����Զ����߹���
	CAN_InitStructure.CAN_AWUM = DISABLE;  //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART = DISABLE;  //��ֹ�����Զ�����
	CAN_InitStructure.CAN_RFLM = DISABLE;  //���Ĳ�����,�µĸ��Ǿɵ�
	CAN_InitStructure.CAN_TXFP = DISABLE;  //���ȼ��ɱ��ı�ʶ������
	CAN_InitStructure.CAN_Mode = mode;	   //ģʽ����
	CAN_InitStructure.CAN_SJW = tsjw;	   //����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = tbs1;	   //Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = tbs2;	   //Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp; //��Ƶϵ��(Fdiv)Ϊbrp+1
	CAN_Init(CAN2, &CAN_InitStructure);	   // ��ʼ��CAN2

	//���ù�����
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
	CAN_FilterInitStructure.CAN_FilterNumber = 21; //EPOS��������
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = EPOS_Motor1_RX << 5;
	CAN_FilterInitStructure.CAN_FilterIdLow = EPOS_Motor2_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = EPOS_Motor3_RX << 5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = EPOS_Motor4_RX << 5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 22; //EPOS������
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

}

//�жϷ�����
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;

	CAN_Receive(CAN2, 0, &RxMessage);
	if (
		(RxMessage.IDE == CAN_Id_Standard)												   //��׼֡��
		&& (RxMessage.IDE == CAN_RTR_Data)												   //����֡��
		&& (RxMessage.DLC == 8) && (RxMessage.StdId > 0x200) && (RxMessage.StdId < 0x209)) /* ���ݳ���Ϊ8   DJ���*/
	{
		u8 id = RxMessage.StdId - 0x201;
		last_update_time[id] = now_update_time[id];
		now_update_time[id] = OSTimeGet();
		motor[id].valueReal.speed = (RxMessage.Data[2] << 8) | (RxMessage.Data[3]);
		motor[id].valueReal.pulseRead = (RxMessage.Data[0] << 8) | (RxMessage.Data[1]);
		motor[id].valueReal.current = (RxMessage.Data[4] << 8) | (RxMessage.Data[5]);
		motor[id].valueReal.tempeture = RxMessage.Data[6];
		motor[id].valueReal.angle = motor[id].valueReal.pulse * 360.f / motor[id].intrinsic.RATIO / motor[id].intrinsic.GearRatio / motor[id].intrinsic.PULSE;
		if (!motor[id].status.clearFlag) //�ϵ��һ�ν��ж����λ�ü�����
		{
			motor[id].status.clearFlag = true;
			motor[id].argum.distance = 0;
		}
	}

	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	CAN_ClearFlag(CAN2, CAN_IT_FMP0);
}

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)
//len:���ݳ���(���Ϊ8)
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN2_Send_Msg(u8 *msg, u8 len)
{
	u8 mbox;
	u16 i = 0;
	CanTxMsg TxMessage;
	TxMessage.StdId = 0x12; // ��׼��ʶ��Ϊ0
	TxMessage.ExtId = 0x12; // ������չ��ʾ����29λ��
	TxMessage.IDE = 0;		// ʹ����չ��ʶ��
	TxMessage.RTR = 0;		// ��Ϣ����Ϊ����֡��һ֡8λ
	TxMessage.DLC = len;	// ������֡��Ϣ
	for (i = 0; i < len; i++)
		TxMessage.Data[i] = msg[i]; // ��һ֡��Ϣ
	mbox = CAN_Transmit(CAN2, &TxMessage);
	i = 0;
	while ((CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF))
		i++; //�ȴ����ͽ���
	if (i >= 0XFFF)
		return 1;
	return 0;
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;

static void Elmo_Feedback_Deel(MesgControlGrpTypeDef *Can_MesgSentList)
{
	Can_MesgSentList->ReceiveNumber += 1;
	Can_MesgSentList->TimeOut = 0;
	Can_MesgSentList->SendSem--;
	Can_MesgSentList->SentQueue.Front = (Can_MesgSentList->SentQueue.Front + 1) % CAN_QUEUESIZE;
}

static void Epos_Feedback_Deel(MesgControlGrpTypeDef *Can_MesgSentList)
{
	Can_MesgSentList->ReceiveNumber += 1;
	Can_MesgSentList->TimeOut = 0;
	Can_MesgSentList->SendSem--;
	Can_MesgSentList->SentQueue.Front = (Can_MesgSentList->SentQueue.Front + 1) % CAN_QUEUESIZE;
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
		if (((rx_message.StdId >= 0x581) && (rx_message.StdId <= 0x585)) && (rx_message.RTR == CAN_RTR_Data)) //EPOS����
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
				Epos_Feedback_Deel(&Can2_eposMesgSentList[id]);
			if (rx_message.Data[0] == 0x4B && rx_message.Data[1] == 0x40 && rx_message.Data[2] == 0x60) //ʹ��
				EPOSmotor[id].enable = rx_message.Data[4] >> 3;
			if (rx_message.Data[0] == 0x4F && rx_message.Data[1] == 0x60 && rx_message.Data[2] == 0x60) //ģʽ
				EPOSmotor[id].mode = rx_message.Data[4];
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x62 && rx_message.Data[2] == 0x60) //�趨λ��
			{
				DecodeS32Data(&EPOSmotor[id].valSet.pulse, &rx_message.Data[4]);
				EPOSmotor[id].valSet.angle = EPOSmotor[id].valSet.pulse * (360.f / EPOSmotor[id].intrinsic.RATIO) / 4.0f / EPOSmotor[id].intrinsic.PULSE;
			}
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x64 && rx_message.Data[2] == 0x60) //ʵ��λ��
			{
				DecodeS32Data(&EPOSmotor[id].valReal.pulse, &rx_message.Data[4]);
				EPOSmotor[id].valReal.angle = EPOSmotor[id].valReal.pulse * (360.f / EPOSmotor[id].intrinsic.RATIO) / 4.0f / EPOSmotor[id].intrinsic.PULSE;
			}
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x6B && rx_message.Data[2] == 0x60) //�趨�ٶ�
				DecodeS32Data(&EPOSmotor[id].valSet.speed, &rx_message.Data[4]);
			if (rx_message.Data[0] == 0x43 && rx_message.Data[1] == 0x6C && rx_message.Data[2] == 0x60) //ʵ���ٶ�
				DecodeS32Data(&EPOSmotor[id].valReal.speed, &rx_message.Data[4]);
			if (rx_message.Data[0] == 0x4B && rx_message.Data[1] == 0x77 && rx_message.Data[2] == 0x60) //ʵ��Ť�ص���
			{
				DecodeS32Data(&EPOSmotor[id].valReal.torque, &rx_message.Data[4]);
				EPOSmotor[id].valReal.current = EPOSmotor[id].valReal.torque / 136.0f;
			}
		}
#endif
#ifdef USE_ELMO
		if (((rx_message.StdId >= 0x281) && (rx_message.StdId <= 0x288)) && (rx_message.RTR == CAN_RTR_Data)) //ELMO��������
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
				(rx_message.Data[0] == 'T' && rx_message.Data[1] == 'C' && (rx_message.Data[3] & BIT6) != 1) |
				(rx_message.Data[0] == 'U' && rx_message.Data[1] == 'M' && (rx_message.Data[3] & BIT6) != 1))
				Elmo_Feedback_Deel(&Can2_elmoMesgSentList[id]);
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
				ELMOmotor[id].valReal.angle = pulse * (360.f / ELMOmotor[id].intrinsic.RATIO) / ELMOmotor[id].intrinsic.PULSE;
			}
			if (rx_message.Data[0] == 'I' && rx_message.Data[1] == 'Q' && (rx_message.Data[3] & BIT6) != 1)
			{
				DecodeS32Data(&ELMOmotor[id].valReal.current, &rx_message.Data[4]);
			}
		}
		if (((rx_message.StdId >= 0x81) && (rx_message.StdId <= 0x88)) && (rx_message.RTR == CAN_RTR_Data)) //ELMO������
		{
			u8 id = rx_message.StdId   - 0x81;
			insertError(Eerror.head, ELMOERROR | ((id + 1) << 4) | EMERGENCY);
			Motor_Emer_Code = (0x1 << 28) | (rx_message.StdId << 16) | (rx_message.Data[0] << 8) | rx_message.Data[1];
		}
#endif
#ifdef USE_VESC
		if ((rx_message.IDE == CAN_ID_EXT) && (rx_message.RTR == CAN_RTR_Data)) //VESC����
		{
			int32_t ind = 0;
			VESCmotor[rx_message.ExtId & 0xff - 1].argum.timeout = 0;
			if ((rx_message.ExtId >> 8) == CAN_PACKET_STATUS)
			{
				VESCmotor[rx_message.ExtId & 0xff - 1].valReal.speed = (s32)(buffer_32_to_float(rx_message.Data, 1e0, &ind) / VESCmotor[rx_message.ExtId & 0xff].instrinsic.POLE_PAIRS);
				VESCmotor[rx_message.ExtId & 0xff - 1].valReal.current = buffer_16_to_float(rx_message.Data, 1e1, &ind);
				VESCmotor[rx_message.ExtId & 0xff - 1].valReal.duty = buffer_16_to_float(rx_message.Data, 1e3, &ind);
			}
		}
#endif
	}
}

u8 CAN2_Receive_Msg(u8 *buf)
{
	u32 i;
	CanRxMsg RxMessage;
	if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0)
		return 0;							  //û�н��յ�����,ֱ���˳�
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage); //��ȡ����
	for (i = 0; i < RxMessage.DLC; i++)
		buf[i] = RxMessage.Data[i];
	return RxMessage.DLC;
}
