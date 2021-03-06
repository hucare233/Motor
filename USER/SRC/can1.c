/*
 * @Descripttion: 
 * @version: 第一版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2021-01-05 21:04:46
 * @FilePath: \MotoPro\USER\SRC\can1.c
 */
/*
 * @Descripttion: 主控通信
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-12-13 19:11:46
 * @FilePath: \MotoPro\USER\SRC\can1.c
 */
#include "can1.h"

static s16 s16TempData[4];
u16 u16TempData[4];
s16 kl_angle;
void CAN1_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  CAN_InitTypeDef CAN_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  CAN_FilterInitTypeDef CAN_FilterInitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

  /* Configure CAN pin: RX A11  TX A12 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* CAN RX interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;         //非时间触发通道模式
  CAN_InitStructure.CAN_ABOM = DISABLE;         //软件对CAN_MCR寄存器的INRQ位置1，随后清0，一旦监测到128次连续11位的隐性位，就退出离线状态
  CAN_InitStructure.CAN_AWUM = DISABLE;         //睡眠模式由软件唤醒
  CAN_InitStructure.CAN_NART = DISABLE;         //DISABLE打开报文自动发送，错误时会重发
  CAN_InitStructure.CAN_RFLM = DISABLE;         //报文不锁定，新的覆盖旧的
  CAN_InitStructure.CAN_TXFP = DISABLE;         //发送FIFO的优先级由标识符决定
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //CAN硬件工作在正常模式

  /* Seting BaudRate */
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; //重新同步跳跃宽度为一个时间单位
  CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq; //时间段1占用8个时间单位
  CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq; //时间段2占用7个时间单位
  CAN_InitStructure.CAN_Prescaler = 3;     //分频系数（Fdiv）
  CAN_Init(CAN1, &CAN_InitStructure);      //初始化CAN1

  /* 波特率计算公式: BaudRate = APB1时钟频率/Fdiv/（SJW+BS1+BS2） */
  /* 42MHz/3/(1+9+4)=1Mhz */

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 1;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0X305 << 5;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0X306 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0X307 << 5;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0X308 << 5;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

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

  CAN_FilterInitStructure.CAN_FilterNumber = 4;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  //	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((uint32_t)0x00010500 << 8) << 3) & 0xffff0000) >> 16;
  //	CAN_FilterInitStructure.CAN_FilterIdLow = (((uint32_t)0x00010500 << 8) << 3) & 0xffff;
  //	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (0xffffff00 << 3) >> 16;
  //	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (0xffffff00 << 3) & 0xffff;
  CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)ID_SELF << 3) & 0xFFFF0000) >> 16;
  CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)ID_SELF << 3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);

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

void feedbackAngle(void)
{
  if (Rear1 == Can1_Sendqueue.Front)
  {
    flag.Can1SendqueueFULL++;
    return;
  }
  else
  {
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].ID = ID_BACK;
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].DLC = 7;
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[0] = 'A';
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[1] = 'S';
    Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[2] = 'K';
    EncodeS16Data(&ELMOmotor[1].valReal.angle, &Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[3]);
    EncodeS16Data(&ELMOmotor[0].valReal.angle, &Can1_Sendqueue.Can_DataSend[Can1_Sendqueue.Rear].Data[5]);
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
#if Id == MOTOR_1_2
      if (( F == MOTOR_0_3
      if ((rx_message.StdId == 0x305) || (rx_message.StdId == 0x308))
#elif ID == MOROE_4_and_2
      if ((rx_message.StdId == 0x305) || (rx_message.StdId == 0x306) || (rx_message.StdId == 0x307))
#elif Id == MOTOR_all
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
          motor[SteeringID].valueSet.angle = s16TempData[SteeringID] / 30.f;
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
          motor[SteeringID].begin = true;    //锁位置，立即运行
          motor[SteeringID].enable = true;   //电机使能
          answer_master(&rx_message);
        }
        else if (rx_message.Data[0] == 'M' && rx_message.Data[1] == 'O' && rx_message.Data[2] == 0) //电机失能
        {
          //				motor[SteeringID].argum.lockPulse = motor[SteeringID].valueReal.pulse;
          motor[SteeringID].enable = false;
          motor[SteeringID].begin = false;
          answer_master(&rx_message);
        }
        else if (rx_message.Data[0] == 'S' && rx_message.Data[1] == 'T') //急停
        {
          motor[SteeringID].begin = false; //锁当前位置
          answer_master(&rx_message);
        }
        
      }
      if (rx_message.StdId == 0x320) //全发
      {
        DecodeS16Data(&s16TempData[0], &rx_message.Data[0]);
        DecodeS16Data(&s16TempData[1], &rx_message.Data[2]);
        DecodeS16Data(&s16TempData[2], &rx_message.Data[4]);
        DecodeS16Data(&s16TempData[3], &rx_message.Data[6]);
#if Id == MOTOR_1_2
        motor[1].valueSet.angle = s16TempData[1] / 30.f * motor[1].intrinsic.GearRatio;
        motor[1].begin = true;
        motor[1].mode = position;
        motor[2].valueSet.angle = s16TempData[2] / 30.f * motor[2].intrinsic.GearRatio;
        motor[2].begin = true;
        motor[2].mode = position;
#elif Id == MOTOR_0_3
        motor[0].valueSet.angle = s16TempData[0] / 30.f * motor[0].intrinsic.GearRatio;
        motor[0].begin = true;
        motor[0].mode = position;
        motor[3].valueSet.angle = s16TempData[3] / 30.f * motor[3].intrinsic.GearRatio;
        motor[3].begin = true;
        motor[3].mode = position;
#elif Id == MOROE_4_and_2
        motor[0].valueSet.angle = s16TempData[0] / 30.f * motor[0].intrinsic.GearRatio;
        motor[0].begin = true;
        motor[0].mode = position;
        motor[1].valueSet.angle = s16TempData[1] / 30.f * motor[1].intrinsic.GearRatio;
        motor[1].begin = true;
        motor[1].mode = position;
        motor[2].valueSet.angle = s16TempData[2] / 30.f * motor[2].intrinsic.GearRatio;
        motor[2].begin = true;
        motor[2].mode = position;
#elif Id == MOTOR_all
        motor[0].valueSet.angle = s16TempData[0] / 30.f;
        motor[0].begin = true;
        motor[0].mode = position;
        motor[1].valueSet.angle = s16TempData[1] / 30.f;
        motor[1].begin = true;
        motor[1].mode = position;
        motor[2].valueSet.angle = s16TempData[2] / 30.f;
        motor[2].begin = true;
        motor[2].mode = position;
        motor[3].valueSet.angle = s16TempData[3] / 30.f;
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
      if ((rx_message.Data[0] == 'K') && (rx_message.Data[1] == 'L'))
      {
        switch (rx_message.Data[2])
        {
        case 1:
        {
          Elmo_Motor_PA(1, -105, 1);
          Elmo_Motor_PA(2, 75, 1);
          Elmo_Motor_PA(3, 75, 1);
          Elmo_Motor_PA(4, -105, 1);
          ELMOmotor[0].valSet.angle = -105;
          ELMOmotor[1].valSet.angle = 75;
          ELMOmotor[2].valSet.angle = 75;
          ELMOmotor[3].valSet.angle = -105;
          // BEGIN_ALL_ELMO
        }
        break;
        case 2: //前腿翻转
        {
          Elmo_Motor_PA(2, 255, 1);
          Elmo_Motor_PA(3, 255, 1);
          ELMOmotor[1].valSet.angle = 255;
          ELMOmotor[2].valSet.angle = 255;
          //BEGIN_ALL_ELMO
        }
        break;
        case 3:
        {
          Elmo_Motor_PA(1, 75, 1);
          Elmo_Motor_PA(4, 75, 1);
          ELMOmotor[0].valSet.angle = 75;
          ELMOmotor[3].valSet.angle = 75;
          //BEGIN_ALL_ELMO
        }
        break;
        case 4:
        {
          Elmo_Motor_PA(1, 0, 1);
          Elmo_Motor_PA(2, 180, 1);
          Elmo_Motor_PA(3, 180, 1);
          Elmo_Motor_PA(4, 0, 1);
          ELMOmotor[0].valSet.angle = 0;
          ELMOmotor[1].valSet.angle = 180;
          ELMOmotor[2].valSet.angle = 180;
          ELMOmotor[3].valSet.angle = 0;
          //BEGIN_ALL_ELMO
        }
        break;
        case 5:
        {
          //Elmo_Motor_PA(1, 18, 1);
          //Elmo_Motor_PA(4,18, 1);
          ELMOmotor[0].valSet.angle = 18;
          ELMOmotor[3].valSet.angle = 18;
          BEGIN_ALL_ELMO
        }
        break;
        case 6:
        {
          Elmo_Motor_PA(1, 0, 1);
          Elmo_Motor_PA(2, 0, 1);
          Elmo_Motor_PA(3, 0, 1);
          Elmo_Motor_PA(4, 0, 1);
          ELMOmotor[0].valSet.angle = 0;
          ELMOmotor[1].valSet.angle = 0;
          ELMOmotor[2].valSet.angle = 0;
          ELMOmotor[3].valSet.angle = 0;
          BEGIN_ALL_ELMO
        }
        break;
        default:
          break;
        }
        DecodeS16Data(&ELMOmotor[0].valSet.speed, &rx_message.Data[3]);
        DecodeS16Data(&ELMOmotor[1].valSet.speed, &rx_message.Data[3]);
        DecodeS16Data(&ELMOmotor[2].valSet.speed, &rx_message.Data[3]);
        DecodeS16Data(&ELMOmotor[3].valSet.speed, &rx_message.Data[3]);
        //DecodeS16Data(&kl_angle, &rx_message.Data[3]);
        //Elmo_Motor_PA(1, kl_angle, 1);
        //Elmo_Motor_PA(2, kl_angle, 1);
        //Elmo_Motor_PA(3, kl_angle, 1);
        //Elmo_Motor_PA(4, kl_angle, 1);
        // ELMOmotor[0].valSet.angle = kl_angle;
        //ELMOmotor[1].valSet.angle = kl_angle;
        // ELMOmotor[2].valSet.angle = kl_angle;
        //ELMOmotor[3].valSet.angle = kl_angle;
        if (rx_message.Data[2] == 1)
        {
          Elmo_Motor_SP(1, ELMOmotor[0].valSet.speed, 1);
          Elmo_Motor_SP(2, ELMOmotor[1].valSet.speed, 1);
          Elmo_Motor_SP(3, ELMOmotor[2].valSet.speed, 1);
          Elmo_Motor_SP(4, ELMOmotor[3].valSet.speed, 1);
        }
        else
        {
          Elmo_Motor_SP(1, ELMOmotor[0].valSet.speed, 1);
          Elmo_Motor_SP(2, ELMOmotor[1].valSet.speed, 1);
          Elmo_Motor_SP(3, ELMOmotor[2].valSet.speed, 1);
          Elmo_Motor_SP(4, ELMOmotor[3].valSet.speed, 1);
        }
        BEGIN_ALL_ELMO
        answer_master(&rx_message);
      }
      else if ((rx_message.Data[0] == 'M') && (rx_message.Data[1] == 'O'))
      {
        switch (rx_message.Data[2])
        {
        case 0:
        {
          DISABLE_ALL_ELMO
        }
        break;
        case 1: //前腿翻转
        {
          ENABLE_ALL_ELMO
        }
        break;
        default:
          break;
        }
      }
      else if ((rx_message.Data[0] == 'G') && (rx_message.Data[1] == 'L'))
      {
        DISABLE_ALL_ELMO
        Elmo_Motor_PX(1, 0, 1);
        Elmo_Motor_PX(2, 0, 1);
        Elmo_Motor_PX(3, 0, 1);
        Elmo_Motor_PX(4, 0, 1);
        ELMOmotor[0].valSet.angle = 0;
        ELMOmotor[1].valSet.angle = 0;
        ELMOmotor[2].valSet.angle = 0;
        ELMOmotor[3].valSet.angle = 0;
      }
			else if (rx_message.Data[0] == 'A' && rx_message.Data[1] == 'S' && rx_message.Data[2] == 'K')
        {
          feedbackAngle();
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
