/*
 * @Descripttion: 串口屏
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-06 20:08:04
 * @FilePath: \MotoPro\USER\SRC\usart2.c
 */
#include "usart2.h"
void USART2_Configuration()
{
  RCC->APB1ENR |= 1 << 17; //使能USART2时钟
  RCC->AHB1ENR |= 1 << 21; //使能DMA1时钟
  GPIO_Set(GPIOA, PIN2 | PIN3, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU);
  GPIO_AF_Set(GPIOA, 2, 7); //复用PA2，PA3到USART2
  GPIO_AF_Set(GPIOA, 3, 7);
  USART2->BRR = (u32)0x016C; //Bound rate set, 115200,72Mhz
  USART2->CR1 = (u32)0x000C; //USART2 setting
  USART2->SR &= ~(1 << 5);   //清除RXNE标志位
  USART2->CR1 |= 1 << 5;     //使能接收中断
  MY_NVIC_Init(4, 1, USART2_IRQn, 3);
  MYDMA_Config(DMA1_Stream6, 4, (u32)&USART2->DR, (u32)usart.TxBuffer_USART2, USART2_Tx_BufferSize);
  DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE); //使能传输完成中断
  MY_NVIC_Init(7, 1, DMA1_Stream6_IRQn, 3);
  USART2->CR3 |= 1 << 7;  //DMA1使能发送接收器
  USART2->CR1 |= 1 << 13; //使能串口1
}

void DMA1_Stream6_IRQHandler(void) //数据传输完成，产生中断，检查是否还有没有传输的数据，继续传输
{
  if (DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6) == SET)
  {
    DMA_ClearFlag(DMA1_Stream6, DMA_IT_TCIF6); //清除中断标志
    DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
    flag.Usart2DMASendFinish = 0;
  }
}

//串口屏
vu8 interface = 0;
u8 USART2_RX_STA = 0;
void USART2_IRQHandler(void)
{
  u8 temp = 0;
  if (USART_GetITStatus(USART2, USART_IT_ORE_RX) != RESET)
  {
    USART_ReceiveData(USART2);
    USART2_RX_STA = 0;
    memset(usart.RxBuffer_USART2, 0, sizeof(usart.RxBuffer_USART2));
  }
  else if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    USART_ClearFlag(USART2, USART_FLAG_RXNE);
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    temp = USART_ReceiveData(USART2);
    if ((USART2_RX_STA & 0x40) != 0)
    {
      if ((USART2_RX_STA & 0x80) == 0)
      {
        if (temp == 0xff)
          USART2_RX_STA |= 0x80;
        else
        {
          usart.RxBuffer_USART2[USART2_RX_STA & 0X3F] = temp;
          USART2_RX_STA++;
          if ((USART2_RX_STA & 0X3F) > 40)
            USART2_RX_STA = 0;
        }
      }
    }
    else if (temp == 0xee)
      USART2_RX_STA |= 0x40;
    if (((USART2_RX_STA & 0x80) != 0)) //串口屏指令判断
    {
      if (usart.RxBuffer_USART2[1] == 0x01)
      {
        interface = usart.RxBuffer_USART2[3];
      }
      switch (usart.RxBuffer_USART2[3])
      {
      case 0x00: //待机界面
      {
        switch (usart.RxBuffer_USART2[5])
        {
        case 0x08:
          RESET_PRO //可以正常使用， 应该是软件bug
              break;
        default:;
        }
      }
      break;
      case 0x01: //1主界面
      {
        break;
      }
      case 0x02: //2主界面
      {
        break;
      }
      case 0x03: //VESC界面
      {
        switch (usart.RxBuffer_USART2[5])
        {
        case 0x03:
        {
          VESCmotor[0].valSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          VESCmotor[1].valSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          VESCmotor[2].valSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          VESCmotor[3].valSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
        }
        break;
        case 0x06: //使能
        {
          VESCmotor[0].enable = 1;
          VESCmotor[1].enable = 1;
          VESCmotor[2].enable = 1;
          VESCmotor[3].enable = 1;
        }
        break;
        case 0x07: //转动
        {
          VESCmotor[0].mode = RPM;
          VESCmotor[1].mode = RPM;
          VESCmotor[2].mode = RPM;
          VESCmotor[3].mode = RPM;
          VESCmotor[0].begin = 1;
          VESCmotor[1].begin = 1;
          VESCmotor[2].begin = 1;
          VESCmotor[3].begin = 1;
        }
        break;
        case 0x08: //刹车
        {
          VESCmotor[0].mode = brake;
          VESCmotor[1].mode = brake;
          VESCmotor[2].mode = brake;
          VESCmotor[3].mode = brake;
        }
        break;
        case 0x0C:
        {
          VESCmotor[0].begin = 0;
          VESCmotor[1].begin = 0;
          VESCmotor[2].begin = 0;
          VESCmotor[3].begin = 0;
        }
        break;
        case 0x0D:
        {
          VESCmotor[0].valSet.duty = atof((char *)(&usart.RxBuffer_USART2[7]));
          VESCmotor[1].valSet.duty = atof((char *)(&usart.RxBuffer_USART2[7]));
          VESCmotor[2].valSet.duty = atof((char *)(&usart.RxBuffer_USART2[7]));
          VESCmotor[3].valSet.duty = atof((char *)(&usart.RxBuffer_USART2[7]));
        }
        }
      }
      case 0x04: //DJ界面
      {
        switch (usart.RxBuffer_USART2[5])
        {
        case 0x03:
          for (int i = 0; i < 8; i++)
            motor[i].enable = false;
          break;
        case 0x04:
          motor[0].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x05:
          motor[1].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x06:
          motor[2].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x07:
          motor[3].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x08:
          motor[0].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x09:
          motor[1].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0A:
          motor[2].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0B:
          motor[3].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0C:
          motor[0].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0D:
          motor[1].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0E:
          motor[2].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0F:
          motor[3].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x11:
          motor[0].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x12:
          motor[1].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x13:
          motor[2].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x14:
          motor[3].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x15:
          motor[0].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x16:
          motor[1].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x17:
          motor[2].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x18:
          motor[3].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x19:
          motor[0].begin = usart.RxBuffer_USART2[8];
          break;
        case 0x1A:
          motor[1].begin = usart.RxBuffer_USART2[8];
          break;
        case 0x1B:
          motor[2].begin = usart.RxBuffer_USART2[8];
          break;
        case 0x1C:
          motor[3].begin = usart.RxBuffer_USART2[8];
          break;
        default:;
        }
      }
      break;
      case 0x05: //DJ界面-副
      {
        switch (usart.RxBuffer_USART2[5])
        {
        case 0x03:
          for (int i = 0; i < 8; i++)
            motor[i].enable = false;
          break;
        case 0x04:
          motor[4].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x05:
          motor[5].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x06:
          motor[6].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x07:
          motor[7].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x08:
          motor[4].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x09:
          motor[5].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0A:
          motor[6].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0B:
          motor[7].valueSet.speed = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0C:
          motor[4].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0D:
          motor[5].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0E:
          motor[6].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x0F:
          motor[7].valueSet.angle = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x11:
          motor[4].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x12:
          motor[5].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x13:
          motor[6].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x14:
          motor[7].limit.posSPlimit = atof((char *)(&usart.RxBuffer_USART2[7]));
          break;
        case 0x15:
          motor[4].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x16:
          motor[5].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x17:
          motor[6].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x18:
          motor[7].enable = usart.RxBuffer_USART2[8];
          break;
        case 0x19:
          motor[4].begin = usart.RxBuffer_USART2[8];
          break;
        case 0x1A:
          motor[5].begin = usart.RxBuffer_USART2[8];
          break;
        case 0x1B:
          motor[6].begin = usart.RxBuffer_USART2[8];
          break;
        case 0x1C:
          motor[7].begin = usart.RxBuffer_USART2[8];
          break;
        default:;
        }
      }
      break;
      default:;
      case 0x07: //EPOS界面
      {
        switch (usart.RxBuffer_USART2[5])
        {
        case 0x03:
        {
          EPOS_RelieveMotor(1, 1);
          EPOS_RelieveMotor(2, 1);
          EPOS_RelieveMotor(3, 1);
          EPOS_RelieveMotor(4, 1);
        }
        break;
        case 0x04:
        {
          EPOS_StartMotor(1, 1);
          EPOS_StartMotor(2, 1);
          EPOS_StartMotor(3, 1);
          EPOS_StartMotor(4, 1);
        }
        break;
        case 0x06:
        {
          EPOS_EnableOperation(1, 1);
          EPOS_EnableOperation(2, 1);
          EPOS_EnableOperation(3, 1);
          EPOS_EnableOperation(4, 1);
        }
        break;
        case 0x07:
        {
          EPOS_AGAINMotorPPM(1, 1);
          EPOS_AGAINMotorPPM(2, 1);
          EPOS_AGAINMotorPPM(3, 1);
          EPOS_AGAINMotorPPM(4, 1);
        }
        break;
        case 0x08:
        {
          EPOS_SetMode(1, 3, 1);
          EPOS_SetMode(2, 3, 1);
          EPOS_SetMode(3, 3, 1);
          EPOS_SetMode(4, 3, 1);
        }
        break;
        case 0x09:
        {
          EPOS_SetMode(1, 1, 1);
          EPOS_SetMode(2, 1, 1);
          EPOS_SetMode(3, 1, 1);
          EPOS_SetMode(4, 1, 1);
        }
        break;
        case 0x0A:
        {
          PPMspeed = atof((char *)(&usart.RxBuffer_USART2[7]));
          EPOS_SetPPMspeed(1, PPMspeed, 1);
          EPOS_SetPPMspeed(2, PPMspeed, 1);
          EPOS_SetPPMspeed(3, PPMspeed, 1);
          EPOS_SetPPMspeed(4, PPMspeed, 1);
        }
        break;
        case 0x0B:
        {
          PPMposition = atof((char *)(&usart.RxBuffer_USART2[7]));
          EPOS_SetPPMposition(1, PPMposition, 1);
          EPOS_SetPPMposition(2, -PPMposition, 1);
          EPOS_SetPPMposition(3, -PPMposition, 1);
          EPOS_SetPPMposition(4, PPMposition, 1);
        }
        break;
        case 0x0C:
        {
          PVMspeed = atof((char *)(&usart.RxBuffer_USART2[7]));
          EPOS_SetPVMspeed(1, PVMspeed, 1);
          EPOS_SetPVMspeed(2, -PVMspeed, 1);
          EPOS_SetPVMspeed(3, -PVMspeed, 1);
          EPOS_SetPVMspeed(4, PVMspeed, 1);
        }
        break;
        case 0x0D:
        {
          EPOS_Halt(1, 1);
          EPOS_Halt(2, 1);
          EPOS_Halt(3, 1);
          EPOS_Halt(4, 1);
        }
        break;
        }
      }
      case 0x08:
      {
        switch (usart.RxBuffer_USART2[5])
        {
        case 0x01:
        {
          ELMOmotor[0].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_UM(1, ELMOmotor[0].mode, 1);
        }
        break;
        case 0x02:
        {
          ELMOmotor[1].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_UM(2, ELMOmotor[1].mode, 1);
        }
        break;
        case 0x03:
        {
          ELMOmotor[2].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_UM(3, ELMOmotor[2].mode, 1);
        }
        break;
        case 0x04:
        {
          ELMOmotor[3].mode = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_UM(4, ELMOmotor[3].mode, 1);
        }
        break;
        case 0x05:
        {
          if (ELMOmotor[0].mode == 2)
          {
            JV = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_JV(1, JV, 1);
          }
          else if (ELMOmotor[0].mode == 5)
          {
            speed = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_SP(1, speed, 1);
          }
        }
        break;
        case 0x06:
        {
          if (ELMOmotor[1].mode == 2)
          {
            Jv = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_JV(2, Jv, 1);
          }
          else if (ELMOmotor[1].mode == 5)
          {
            speed = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_SP(2, speed, 1);
          }
        }
        break;
        case 0x07:
        {
          if (ELMOmotor[2].mode == 2)
          {
            JV = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_JV(3, Jv, 1);
          }
          else if (ELMOmotor[2].mode == 5)
          {
            speed = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_SP(3, speed, 1);
          }
        }
        break;
        case 0x08:
        {
          if (ELMOmotor[3].mode == 2)
          {
            JV = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_JV(4, Jv, 1);
          }
          else if (ELMOmotor[3].mode == 5)
          {
            speed = atof((char *)(&usart.RxBuffer_USART2[7]));
            Elmo_Motor_SP(4, speed, 1);
          }
        }
        break;
        case 0x09:
        {
          PA = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PA(1, Pa, 1);
        }
        break;
        case 0x0A:
        {
          PA = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PA(2, Pa, 1);
        }
        break;
        case 0x0B:
        {
          PA = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PA(3, Pa, 1);
        }
        break;
        case 0x0C:
        {
          PA = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PA(4, Pa, 1);
        }
        break;
        case 0x0D:
        {
          PX = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PX(1, Px, 1);
        }
        break;
        case 0x0E:
        {
          PX = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PX(2, Px, 1);
        }
        break;
        case 0x0F:
        {
          PX = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PX(3, Px, 1);
        }
        break;
        case 0x10:
        {
          PX = atof((char *)(&usart.RxBuffer_USART2[7]));
          Elmo_Motor_PX(4, Px, 1);
        }
        break;
        case 0x15:
        {
          ELMOmotor[0].enable = usart.RxBuffer_USART2[8];
          Elmo_Motor_Enable_Or_Disable(1, ELMOmotor[0].enable, 1);
        }
        break;
        case 0x11:
        {
          ELMOmotor[1].enable = usart.RxBuffer_USART2[8];
          Elmo_Motor_Enable_Or_Disable(2, ELMOmotor[1].enable, 1);
        }
        break;
        case 0x12:
        {
          ELMOmotor[2].enable = usart.RxBuffer_USART2[8];
          Elmo_Motor_Enable_Or_Disable(3, ELMOmotor[2].enable, 1);
        }
        break;
        case 0x13:
        {
          ELMOmotor[3].enable = usart.RxBuffer_USART2[8];
          Elmo_Motor_Enable_Or_Disable(4, ELMOmotor[3].enable, 1);
        }
        break;
        case 0x16:
        {
          Elmo_Motor_BG(1, 1);
        }
        break;
        case 0x14:
        {
          Elmo_Motor_BG(2, 1);
        }
        break;
        case 0x18:
        {
          Elmo_Motor_BG(3, 1);
        }
        break;
        case 0x17:
        {
          Elmo_Motor_BG(4, 1);
        }
        break;
        case 0x19:
        {
          Elmo_Motor_ST(1, 1);
          Elmo_Motor_ST(2, 1);
          Elmo_Motor_ST(3, 1);
          Elmo_Motor_ST(4, 1);
        }
        break;
        }
      }
      }
      USART2_RX_STA = 0;
      memset(usart.RxBuffer_USART2, 0, sizeof(usart.RxBuffer_USART2));
    }
  }
}

static void USART2_Send(u8 count)
{
  if (!flag.Usart2DMASendFinish) //若上一次传输未完成，则舍弃本次传输
  {
    DMA_SetCurrDataCounter(DMA1_Stream6, count);
    DMA_Cmd(DMA1_Stream6, ENABLE);
    flag.Usart2DMASendFinish = 1;
  }
}

char str_temp[32];
void UsartLCDshow(void)
{
  u8 i = 0; //用于串口数据包的下标

  /*****主界面****/
  usart.TxBuffer_USART2[i++] = 0xee;
  usart.TxBuffer_USART2[i++] = 0xb1;
  usart.TxBuffer_USART2[i++] = 0x10;
  usart.TxBuffer_USART2[i++] = 0x00;
  usart.TxBuffer_USART2[i++] = 0x00;
  usart.TxBuffer_USART2[i++] = 0x00;
  usart.TxBuffer_USART2[i++] = 0x05;
  usart.TxBuffer_USART2[i++] = 0x01;

  usart.TxBuffer_USART2[i++] = 0xff;
  usart.TxBuffer_USART2[i++] = 0xfc;
  usart.TxBuffer_USART2[i++] = 0xff;
  usart.TxBuffer_USART2[i++] = 0xff;

  switch (interface)
  {
  case 0x01: /****P车主界面****/
  {
    ;
  }
  break;
  case 2: /****T车主界面****/
  {
    ;
  }
  break;
  case 3:
  {
  }
  break;
  case 4: /****DJ界面****/
  {
    usart.TxBuffer_USART2[i++] = 0xee;
    usart.TxBuffer_USART2[i++] = 0xb1;
    usart.TxBuffer_USART2[i++] = 0x12;
    usart.TxBuffer_USART2[i++] = 0x00;
    usart.TxBuffer_USART2[i++] = 0x04;

    if (motor[0].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x04;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[0].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x08;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[0].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0C;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[0].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x11;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[0].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x15;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[0].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x19;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[0].begin;
    }
    if (motor[1].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x05;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[1].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x09;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[1].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0D;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[1].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x12;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[1].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x16;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[1].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x1A;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[1].begin;
    }
    if (motor[2].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x06;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[2].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0A;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[2].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0E;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[2].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x13;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[2].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x17;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[2].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x1B;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[2].begin;
    }
    if (motor[3].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x07;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[3].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0B;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[3].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0F;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[3].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x14;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[3].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x18;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[3].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x1C;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[3].begin;
    }
    usart.TxBuffer_USART2[i++] = 0x00;
    usart.TxBuffer_USART2[i++] = 0x10;
    usart.TxBuffer_USART2[i++] = 0x00;
    sprintf(str_temp, "%x", Eerror.head->next->code);
    usart.TxBuffer_USART2[i++] = strlen(str_temp);
    strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
    i += strlen(str_temp);

    usart.TxBuffer_USART2[i++] = 0xff;
    usart.TxBuffer_USART2[i++] = 0xfc;
    usart.TxBuffer_USART2[i++] = 0xff;
    usart.TxBuffer_USART2[i++] = 0xff;
  }
  break;
  case 5: /****DJ界面-副****/
  {
    usart.TxBuffer_USART2[i++] = 0xee;
    usart.TxBuffer_USART2[i++] = 0xb1;
    usart.TxBuffer_USART2[i++] = 0x12;
    usart.TxBuffer_USART2[i++] = 0x00;
    usart.TxBuffer_USART2[i++] = 0x05;

    if (motor[4].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x04;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[4].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x08;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[4].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0C;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[4].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x11;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[4].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x15;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[4].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x19;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[4].begin;
    }
    if (motor[5].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x05;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[5].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x09;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[5].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0D;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[5].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x12;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[5].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x16;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[5].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x1A;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[5].begin;
    }
    if (motor[6].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x06;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[6].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0A;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[6].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0E;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[6].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x13;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[6].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x17;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[6].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x1B;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[6].begin;
    }
    if (motor[7].mode != 0)
    {
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x07;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[7].mode);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0B;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[7].valueReal.speed);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x0F;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[7].valueReal.angle);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x14;
      usart.TxBuffer_USART2[i++] = 0x00;
      sprintf(str_temp, "%d", motor[7].limit.posSPlimit);
      usart.TxBuffer_USART2[i++] = strlen(str_temp);
      strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
      i += strlen(str_temp);

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x18;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[7].enable;

      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x1C;
      usart.TxBuffer_USART2[i++] = 0x00;
      usart.TxBuffer_USART2[i++] = 0x01;
      usart.TxBuffer_USART2[i++] = motor[7].begin;
    }

    usart.TxBuffer_USART2[i++] = 0x00;
    usart.TxBuffer_USART2[i++] = 0x10;
    usart.TxBuffer_USART2[i++] = 0x00;
    sprintf(str_temp, "%x", Eerror.head->next->code);
    usart.TxBuffer_USART2[i++] = strlen(str_temp);
    strcpy((char *)(&usart.TxBuffer_USART2[i]), str_temp);
    i += strlen(str_temp);

    usart.TxBuffer_USART2[i++] = 0xff;
    usart.TxBuffer_USART2[i++] = 0xfc;
    usart.TxBuffer_USART2[i++] = 0xff;
    usart.TxBuffer_USART2[i++] = 0xff;
  }
  break;
  /*case 8://elmo界面
		{
		 usart.TxBuffer_USART2[i++]=0xee;
    usart.TxBuffer_USART2[i++]=0xb1;	
    usart.TxBuffer_USART2[i++]=0x12;	
    usart.TxBuffer_USART2[i++]=0x00;	
    usart.TxBuffer_USART2[i++]=0x08;
    
    if(ELMOmotor[0].mode != 0)
    {
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x01;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[0].mode);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);    
   
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x05;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[0].valReal.speed);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);    
   
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x09;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[0].valReal.pulse);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);
     
		 
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x15;
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x01;
    usart.TxBuffer_USART2[i++]=ELMOmotor[0].enable;
    
    }
    if(ELMOmotor[1].mode != 0)
    {
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x02;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[1].mode);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);    
      
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x06;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[1].valReal.speed);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);    
 
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x0A;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[1].valReal.pulse);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);
       
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x11;
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x01;
    usart.TxBuffer_USART2[i++]=ELMOmotor[1].enable;
    
    }
    if(ELMOmotor[2].mode != 0)
    {
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x03;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[2].mode);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);    
    
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x07;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[2].valReal.speed);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);   
      
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x0B;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[2].valReal.pulse);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);
   
    
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x12;
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x01;
    usart.TxBuffer_USART2[i++]=ELMOmotor[2].enable;
    
    }
    if(ELMOmotor[3].mode != 0)
    {
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x04;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[3].mode);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);    
      
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x08;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[3].valReal.speed);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);  
   
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x0C;
    usart.TxBuffer_USART2[i++]=0x00;
    sprintf(str_temp,"%d",ELMOmotor[3].valReal.angle);
    usart.TxBuffer_USART2[i++]=strlen(str_temp);
    strcpy((char*)(&usart.TxBuffer_USART2[i]),str_temp);
    i += strlen(str_temp);
    
    
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x13;
    usart.TxBuffer_USART2[i++]=0x00;
    usart.TxBuffer_USART2[i++]=0x01;
    usart.TxBuffer_USART2[i++]=ELMOmotor[3].enable;
    
    }
    
    usart.TxBuffer_USART2[i++]=0xff;
    usart.TxBuffer_USART2[i++]=0xfc;
    usart.TxBuffer_USART2[i++]=0xff;
    usart.TxBuffer_USART2[i++]=0xff;
		
		}break;*/
  default:;
  }
  USART2_Send(i);
}
