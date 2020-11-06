#ifndef _PARAM_H_
#define _PARAM_H_

#include "motor.h"
#include "led.h"
#include "buffer.h"
#include "vesc.h"
#include "stm32f4xx.h"
#include "elmo.h"
#define RESET_PRO {__set_FAULTMASK(1);NVIC_SystemReset();}
typedef _Bool(*MatchFunc) (const void *key1, const void *key2);

/**
 * @description: serial communication buffer size
 */
typedef enum
{
  USART1_Tx_BufferSize = 256, //串口一发送数据长度
  USART2_Tx_BufferSize = 256, //串口二发送数据长度
  USART3_Tx_BufferSize = 32,  //串口三发送数据长度
  USART1_Rx_BufferSize = 32, //串口一接收数据长度（接收中断内使用）
  USART2_Rx_BufferSize = 32, //串口二接收数据长度（接收中断内使用）
  USART3_Rx_BufferSize = 32, //串口三接收数据长度（接收中断内使用）
} Usart_Enum;

/**
 * @description: serial communication data buffer struct
 */
typedef struct{
  u8 TxBuffer_USART1[USART1_Tx_BufferSize]; //串口一发送数据
  u8 RxBuffer_USART1[USART1_Rx_BufferSize]; //串口一接收数据

  u8 TxBuffer_USART2[USART2_Tx_BufferSize]; //串口二发送数据
  u8 RxBuffer_USART2[USART2_Rx_BufferSize]; //串口二接收数据

  u8 TxBuffer_USART3[USART3_Tx_BufferSize]; //串口三发送数据
  u8 RxBuffer_USART3[USART3_Rx_BufferSize]; //串口三接收数据
} Usart_Struct;

/****标志位结构体****/
typedef struct{
    bool valve[4];                     //当前电磁阀状态
    volatile bool Can1CtrlList_Enable; // CAN1报文控制表启动标志位
    volatile bool Can2CtrlList_Enable; // CAN2报文控制表启动标志位
    volatile bool Can1SendqueueFULL;   // CAN1发送队列满
    volatile bool Can2SendqueueFULL;   // CAN2发送队列满
	  volatile bool VESCSendqueueFULL;   //VESC发送队列满
    volatile bool CanSendqueueEMPTY;   // CAN队列为空
    volatile bool Usart1DMASendFinish; // Usart1_DMA传输
    volatile bool Usart2DMASendFinish; // Usart2_DMA传输
	  volatile bool Usart3DMASendFinish; // Usart2_DMA传输
} FlagTypeDef;

/****踢球结构体****/
typedef struct{
  bool prepareOK;
  u8 cnt;
  s16 waittime;
  s16 waitCnt;
  bool isAutoKick;
  bool init;
  bool ok;//踢球结束
}KickTypeDef;

//第一位
#define SYSTEMERROR   0x0000
#define DJERROR       0x0001
#define ELMOERROR     0x0002
#define VESCERROR     0x0003
#define EPOSERROR     0x0004
//第三位
#define TIMEOUT       0x0100
#define STUCK         0x0200
#define EMERGENCY     0x0300

/****错误示显链表****/
typedef struct _ErrorList{
  u16 code;
  struct _ErrorList* next;
}ErrorList;

typedef struct{
  u8 size;
  ErrorList* head;
}ErrorTypeDef;

extern u8 kickBall;
extern ErrorTypeDef Eerror;
extern KickTypeDef kick[3];
extern Usart_Struct usart;
extern FlagTypeDef flag;

void param_Init(void);
void Flag_Init(void);
void ChangeData(u8* buff2,u8* buff1);
ErrorList* creatError(void);
void deleteError(ErrorList* list, ErrorList* listdel);
void insertError(ErrorList* list, uc16 errorcode);
ErrorList* ErrorFind(ErrorList* list, uc16 errorcode);

#endif
