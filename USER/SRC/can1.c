/*
 * @Descripttion: 主控通信
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-13 22:04:29
 * @FilePath: \MotoPro\USER\SRC\can1.c
 */
#include "can1.h"

static s16 s16TempData[4];
u16 u16TempData[4];

void CAN1_Configuration(void)
{
  CAN_InitTypeDef CAN_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;         //非时间触发通道模式
  CAN_InitStructure.CAN_ABOM = DISABLE;         //软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
  CAN_InitStructure.CAN_AWUM = DISABLE;         //睡眠模式由软件唤醒
  CAN_InitStructure.CAN_NART = DISABLE;         //禁止报文自动发送，即只发送一次，无论结果如何
  CAN_InitStructure.CAN_RFLM = DISABLE;         //报文不锁定，新的覆盖旧的
  CAN_InitStructure.CAN_TXFP = DISABLE;         //发送FIFO的优先级由标识符决定
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; // CAN硬件工作在正常模式

  /* Seting BaudRate */
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //重新同步跳跃宽度为一个时间单位
  CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; //时间段1占用8个时间单位
  CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq; //时间段2占用7个时间单位
  CAN_InitStructure.CAN_Prescaler = 3;     //分频系数（Fdiv）
  CAN_Init(CAN1, &CAN_InitStructure);      //初始化CAN1

  /* CAN filter init */
#ifdef SteeringMotor
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
#if ID_SELF == MOTOR_0_3
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X305 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X308 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x320 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0 << 5;
#elif ID_SELF == MOTOR_1_2
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X306 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X307 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x320 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x309 << 5;
#elif ID_SELF == MOTOR_all
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X305 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X306 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X307 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X320 << 5;
#elif ID_SELF == MOROE_4_and_2
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X305 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X306 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X307 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X308 << 5;
#endif
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

#if ID_SELF == MOROE_4_and_2
  CAN_FilterInitStructure.CAN_FilterNumber = 3;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X320 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X301 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X302 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x303 << 5;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
#endif
	CAN_FilterInitStructure.CAN_FilterNumber = 4;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
//	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((uint32_t)0x00010500 << 8) << 3) & 0xffff0000) >> 16;
//	CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)0x00010500 << 8) << 3) & 0xffff;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0xffffff00 << 3) >> 16;
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (0xffffff00 << 3) & 0xffff;
	      CAN_FilterInitStructure.CAN_FilterIdHigh   = (((u32)0x00010500<<3)&0xFFFF0000)>>16;
      CAN_FilterInitStructure.CAN_FilterIdLow   = (((u32)0x00010500<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
      CAN_FilterInitStructure.CAN_FilterMaskIdHigh  = 0xFFFF;
      CAN_FilterInitStructure.CAN_FilterMaskIdLow   = 0xFFFF;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
#if ID_SELF == MOTOR_0_3
#ifdef USE_ELMO
  CAN_FilterInitStructure.CAN_FilterNumber = 1;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = Elmo_Motor1_RX << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = Elmo_Motor2_RX << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = Elmo_Motor3_RX << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = Elmo_Motor4_RX << 5;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
#elif defined USE_EPOS
  CAN_FilterInitStructure.CAN_FilterNumber = 1;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = EPOS_Motor1_RX << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = EPOS_Motor2_RX << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = EPOS_Motor3_RX << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = EPOS_Motor4_RX << 5;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  CAN_FilterInitStructure.CAN_FilterNumber = 2;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = EPOS_Motor1_Init << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = EPOS_Motor2_Init << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = EPOS_Motor3_Init << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = EPOS_Motor4_Init << 5;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
#endif
#endif
#endif
#ifdef ActionMotor
  CAN_FilterInitStructure.CAN_FilterNumber = 0; //屏蔽位模式
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = ((ID_SELF << 3) & 0xFFFF0000) >> 16;
  CAN_FilterInitStructure.CAN_FilterIdLow = (ID_SELF << 3) & 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0xFFFF00 << 3) >> 16;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = (0xFFFF << 3) & 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

  CAN_FilterInitStructure.CAN_FilterNumber = 1; //屏蔽位模式
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = ((BROADCAST_ID << 3) & 0xFFFF0000) >> 16;
  CAN_FilterInitStructure.CAN_FilterIdLow = (BROADCAST_ID << 3) & 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0xFFFF00 << 3) >> 16;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = (0xFFFF << 3) & 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
#endif

  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}
/****接收反馈原报文写入队列****/
static void answer_master(CanRxMsg *rx_message)
{
  if (Rear1 == Can1_Sendqueue.Front)
  {
    flag.Can1SendqueueFULL++;
    return;
  }
  else
  {
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].ID = rx_message->StdId - 0X80;
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].DLC = rx_message->DLC;
    memcpy(&Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[0], &rx_message->Data[0], rx_message->DLC);
  }
  Can1_Sendqueue.Rear = Rear1;
}

void feedbackAngle(u16 motorID)
{

  if (Rear1 == Can1_Sendqueue.Front)
  {
    flag.Can1SendqueueFULL++;
    return;
  }
  else
  {
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].ID = motorID - 0x80;
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].DLC = 5;
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[0] = 'B';
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[1] = 'B';
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[2] = 'G';
    u8 id = motorID - 0x305;
    s16TempData[id] = motor[id].valueReal.angle * 30.f / motor[id].intrinsic.GearRatio;
    EncodeS16Data(&s16TempData[id], &Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[3]);
  }
  Can1_Sendqueue.Rear = Rear1;
}

void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg rx_message;
  if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
  {
    CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    CAN_ClearFlag(CAN1, CAN_IT_FMP0);
    CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
    /*	#ifdef VER
			if((u8)rxmsg.Data[1] >= (u8)('A'+0x40))		
			{
				CAN_Procedure.status |= BIT0;//当前报文拥有流程号
				CAN_Procedure.order_now = (u8)rxmsg.Data[rxmsg.DLC - 1];//流程号为报文最后一个字节	
				if(CAN_Procedure.status & BIT7)
				{
					CAN_Procedure.order_valid = CAN_Procedure.order_now;			
					CAN_Procedure.order_valid++;	
					CAN_Procedure.status = 0x03;				
				}
				else
				{
					CAN_Procedure.status = 0x01;							
					if(CAN_Procedure.order_valid == CAN_Procedure.order_now)
					{
						CAN_Procedure.status |= BIT1;				
						CAN_Procedure.order_valid ++;
					}
					else
					{
						CAN_Procedure.status &= ~BIT1;					
						if(CAN_Procedure.order_valid > 127)
						{
							if((CAN_Procedure.order_now >= (CAN_Procedure.order_valid - 128)) && (CAN_Procedure.order_now < CAN_Procedure.order_valid))						
								CAN_Procedure.status &= ~BIT2;
							else
								CAN_Procedure.status |= BIT2;	
						}
						else
						{
							if((CAN_Procedure.order_now <= (CAN_Procedure.order_valid + 127)) && (CAN_Procedure.order_now > CAN_Procedure.order_valid))							
								CAN_Procedure.status |= BIT2;	
							else
								CAN_Procedure.status &= ~BIT2;	
						}
					}
				}
			}
			else
			{
				CAN_Procedure.status &= ~BIT0;
			}
		#else		
			CAN_Procedure.status = (BIT0|BIT1);//流程号关闭，则默认具有正确流程号
		#endif
		*/
#ifdef SteeringMotor //转向电机报文处理
    if ((rx_message.IDE == CAN_Id_Standard) && (rx_message.RTR == CAN_RTR_Data))
    {
#ifdef PassRobot
#if ID_SELF == MOTOR_1_2
      if (( F == MOTOR_0_3
      if ((rx_message.StdId == 0x305) || (rx_message.StdId == 0x308))
#elif ID_SELF == MOTOR_all
      if ((rx_message.StdId == 0x305) || (rx_message.StdId == 0x306) || (rx_message.StdId == 0x307))
#elif ID_SELF == MOROE_4_and_2
      if ((rx_message.StdId == 0x305) || (rx_message.StdId == 0x306) || (rx_message.StdId == 0x307) || (rx_message.StdId == 0x308)) //任务要求
#endif
      {
        u8 SteeringID = rx_message.StdId - 0x305;
        if (rx_message.Data[0] == 'C' && rx_message.Data[1] == 'W' && rx_message.Data[2] == 'H' && rx_message.Data[3] == 'U') //自检
        {
          answer_master(&rx_message);
        }
        else if (rx_message.Data[0] == 'C' && rx_message.Data[1] == 'H') //旋转
        {
          DecodeS16Data(&s16TempData[SteeringID], &rx_message.Data[2]);
          motor[SteeringID].valueSet.angle = s16TempData[SteeringID] / 30.f * motor[SteeringID].intrinsic.GearRatio;
          motor[SteeringID].begin = true;
        }
        else if (rx_message.Data[0] == 'S' && rx_message.Data[1] == 'P') //速度设定
        {
          DecodeS16Data(&u16TempData[SteeringID], &rx_message.Data[2]);
          motor[SteeringID].limit.posSPlimit = u16TempData[SteeringID];
          answer_master(&rx_message);
        }
        else if (rx_message.Data[0] == 'M' && rx_message.Data[1] == 'O' && rx_message.Data[2] == 1) //电机使能
        {
          motor[SteeringID].mode = position; //位置模式
          motor[SteeringID].begin = true;    //不锁位置，立即运行
          motor[SteeringID].enable = true;   //电机使能
          answer_master(&rx_message);
        }
        else if (rx_message.Data[0] == 'M' && rx_message.Data[1] == 'O' && rx_message.Data[2] == 0) //电机失能
        {
          motor[SteeringID].enable = false;
          motor[SteeringID].begin = false;
          answer_master(&rx_message);
        }
        else if (rx_message.Data[0] == 'S' && rx_message.Data[1] == 'T') //急停
        {
          motor[SteeringID].begin = false; //锁当前位置
          answer_master(&rx_message);
        }
        else if (rx_message.Data[0] == 'B' && rx_message.Data[1] == 'B' && rx_message.Data[2] == 'G')
        {
          feedbackAngle(rx_message.StdId);
        }
      }
      if (rx_message.StdId == 0x320) //全发
      {
        DecodeS16Data(&s16TempData[0], &rx_message.Data[0]);
        DecodeS16Data(&s16TempData[1], &rx_message.Data[2]);
        DecodeS16Data(&s16TempData[2], &rx_message.Data[4]);
        DecodeS16Data(&s16TempData[3], &rx_message.Data[6]);
#if ID_SELF == MOTOR_1_2
        motor[1].valueSet.angle = s16TempData[1] / 30.f * motor[1].intrinsic.GearRatio;
        motor[1].begin = true;
        motor[1].mode = position;
        motor[2].valueSet.angle = s16TempData[2] / 30.f * motor[2].intrinsic.GearRatio;
        motor[2].begin = true;
        motor[2].mode = position;
#elif ID_SELF == MOTOR_0_3
        motor[0].valueSet.angle = s16TempData[0] / 30.f * motor[0].intrinsic.GearRatio;
        motor[0].begin = true;
        motor[0].mode = position;
        motor[3].valueSet.angle = s16TempData[3] / 30.f * motor[3].intrinsic.GearRatio;
        motor[3].begin = true;
        motor[3].mode = position;
#elif ID_SELF == MOTOR_all
        motor[0].valueSet.angle = s16TempData[0] / 30.f * motor[0].intrinsic.GearRatio;
        motor[0].begin = true;
        motor[0].mode = position;
        motor[1].valueSet.angle = s16TempData[1] / 30.f * motor[1].intrinsic.GearRatio;
        motor[1].begin = true;
        motor[1].mode = position;
        motor[2].valueSet.angle = s16TempData[2] / 30.f * motor[2].intrinsic.GearRatio;
        motor[2].begin = true;
        motor[2].mode = position;
#elif ID_SELF == MOROE_4_and_2
        motor[0].valueSet.angle = s16TempData[0] / 30.f * motor[0].intrinsic.GearRatio;
        motor[0].begin = true;
        motor[0].mode = position;
        motor[1].valueSet.angle = s16TempData[1] / 30.f * motor[1].intrinsic.GearRatio;
        motor[1].begin = true;
        motor[1].mode = position;
        motor[2].valueSet.angle = s16TempData[2] / 30.f * motor[2].intrinsic.GearRatio;
        motor[2].begin = true;
        motor[2].mode = position;
        motor[3].valueSet.angle = s16TempData[2] / 30.f * motor[3].intrinsic.GearRatio;
        motor[3].begin = true;
        motor[3].mode = position;
#endif
        answer_master(&rx_message);
      }
			
#if ID_SELF == MOTOR_0_3 | ID_SELF == MOTOR_all
#ifdef USE_ELMO
      if ((rx_message.StdId == Elmo_Motor1_RX) || (rx_message.StdId == Elmo_Motor2_RX) || (rx_message.StdId == Elmo_Motor3_RX) || (rx_message.StdId == Elmo_Motor4_RX))
      {
        if ((rx_message.Data[0] == 'M' && rx_message.Data[1] == 'O' && (rx_message.Data[3] & BIT6) != 1))
        {
          u8 ElmoID = rx_message.StdId - 0x281;
          ELMOmotor[ElmoID].enable = 1;
        }
      }
#elif defined USE_EPOS
      if ((rx_message.StdId == Elmo_Motor1_RX) || (rx_message.StdId == Elmo_Motor2_RX) || (rx_message.StdId == Elmo_Motor3_RX) || (rx_message.StdId == Elmo_Motor4_RX))
      {
      }
#endif
#endif
#elif defined TryRobot

#endif
    }
		if ((rx_message.IDE == CAN_Id_Extended) && (rx_message.ExtId == 0X00010500))
      {
        if ((rx_message.Data[1] == 'k') && (rx_message.Data[2] == 'L'))
        {
          if (rx_message.Data[3] == 1) //车抬起
          {
            motor[4].valueSet.angle = 170*motor[4].intrinsic.GearRatio;
            motor[5].valueSet.angle = -170*motor[5].intrinsic.GearRatio;
            motor[6].valueSet.angle = 170*motor[6].intrinsic.GearRatio;
            motor[7].valueSet.angle = -170*motor[7].intrinsic.GearRatio;
          }
          if (rx_message.Data[3] == 0) //车放下
          {
            motor[4].valueSet.angle = 95*motor[4].intrinsic.GearRatio;
            motor[5].valueSet.angle = -95*motor[5].intrinsic.GearRatio;
            motor[6].valueSet.angle = 95*motor[6].intrinsic.GearRatio;
            motor[7].valueSet.angle = -95*motor[7].intrinsic.GearRatio;
          }
          DecodeS16Data(&motor[4].limit.posSPlimit, &rx_message.Data[4]);
          DecodeS16Data(&motor[5].limit.posSPlimit, &rx_message.Data[4]);
          DecodeS16Data(&motor[6].limit.posSPlimit, &rx_message.Data[4]);
          DecodeS16Data(&motor[7].limit.posSPlimit, &rx_message.Data[4]);
          answer_master(&rx_message);
        }
      }
#elif defined ActionMotor //执行电机报文处理
    if ((rx_message.IDE == CAN_Id_Extended) && (rx_message.RTR == CAN_RTR_Data))
    {
      if (rx_message.Data[0] == 0x00) //广播帧
      {
        if ((rx_message.Data[1] == 'C') && (rx_message.Data[2] == 'W') && (rx_message.Data[3] == 'H') && (rx_message.Data[4] == 'U'))
          answer_master(&rx_message);
      }
      if (rx_message.Data[0] == 0x03)
      {
#ifdef PassRobot

#elif defined TryRobot

#endif
      }
    }
#endif
  }
}
