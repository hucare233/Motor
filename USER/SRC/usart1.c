/*
 * @Descripttion: 虚拟示波器
 * @version: 第二版
 * @Author: 叮咚蛋
 * @Date: 2020-10-17 14:52:41
 * @LastEditors: 叮咚蛋
 * @LastEditTime: 2020-11-06 20:07:58
 * @FilePath: \MotoPro\USER\SRC\usart1.c
 */
#include "usart1.h"
uint8_t aTxBuffer_UART1[51] = {0}; //一次传输数据量不要大于52字节
void USART1_Configuration(void)
{
    RCC->APB2ENR |= 1 << 4;  //使能USART1时钟
    RCC->AHB1ENR |= 1 << 22; //使能DMA2时钟
    GPIO_Set(GPIOA, PIN9 | PIN10, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU);
    GPIO_AF_Set(GPIOA, 9, 7); //复用PA9，PA10到USART1
    GPIO_AF_Set(GPIOA, 10, 7);
    USART1->BRR = (u32)0x02D9; //波特率设置,115200
    USART1->CR1 = (u32)0x020C; //USART1 setting, 使能接收中断
    USART1->SR &= ~(1 << 5);   //清除RXNE标志位
    MY_NVIC_Init(6, 1, USART1_IRQn, 3);
    MYDMA_Config(DMA2_Stream7, 4, (u32)&USART1->DR, (u32)usart.TxBuffer_USART1, USART1_Tx_BufferSize);
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE); //使能传输完成中断
    MY_NVIC_Init(7, 0, DMA2_Stream7_IRQn, 3);
    USART1->CR3 |= 1 << 7;  //DMA2使能发送接收器
    USART1->CR1 |= 1 << 13; //使能串口1
}

void USART1_Send(INT8U *data, INT8U count)
{
    if (!flag.Usart1DMASendFinish) //若上一次传输未完成，则舍弃本次传输
    {
        memcpy(usart.TxBuffer_USART1, data, count);
        DMA_SetCurrDataCounter(DMA2_Stream7, count);
        DMA_Cmd(DMA2_Stream7, ENABLE);
        flag.Usart1DMASendFinish = 1;
    }
}

void DMA2_Stream7_IRQHandler(void) //数据传输完成，产生中断，检查是否还有没有传输的数据，继续传输
{
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) == SET)
    {
        DMA_ClearFlag(DMA2_Stream7, DMA_IT_TCIF7); //清除中断标志
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
        flag.Usart1DMASendFinish = 0;
    }
}
