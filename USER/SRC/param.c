#include "param.h"

/* 踢球常量定义 */
KickTypeDef kick[3];
u8 kickBall = 1;
ErrorTypeDef Eerror;
Usart_Struct usart;
FlagTypeDef flag;
void param_Init(void)
{
  ELMO_Motor_Init();
  ELMO_Init(CAN2);
  EPOS_Motor_Init();
  EPOS_Init(CAN2);
  Motor_Init();
  VESCInit();
  Flag_Init();
  {
    for (int i = 0; i < 2; i++)
    {
      kick[i].cnt = 0;
      kick[i].isAutoKick = false;
      kick[i].waittime = 1000;
      kick[i].waitCnt = 0;
    }
  }
  Eerror.head = creatError();
}

void Flag_Init(void)
{
  flag.Can2CtrlList_Enable = true;
}

/* 改变16位数据高低顺序 */
void ChangeData(u8 *buff2, u8 *buff1)
{
  u8 temp = 0;
  temp = *buff1;
  *buff1 = *buff2;
  *buff2 = temp;
}

/* 创建一个空错误链表 */
ErrorList *creatError(void)
{
  ErrorList *head;
  head = (ErrorList *)malloc(sizeof(ErrorList));
  head->next = NULL;
  return head;
}

void deleteError(ErrorList *list, ErrorList *listdel)
{
  ErrorList *tpre = list;
  while (tpre != NULL && (tpre->next) != listdel)
  {
    tpre = tpre->next;
  }
  if (listdel != NULL)
  {
    tpre->next = listdel->next;
    free(listdel);
    Eerror.size--;
  }
}

/* 顺序插入节点 */
void insertError(ErrorList *list, uc16 errorcode)
{
  ErrorList *t = list, *in;
  if (ErrorFind(t, errorcode) != NULL)
    return;
  u8 i = 0;
  while (i < Eerror.size && t != NULL)
  {
    t = t->next;
    i++;
  }
  if (t != NULL)
  {
    in = (ErrorList *)malloc(sizeof(ErrorList));
    in->code = errorcode;
    in->next = t->next;
    t->next = in;
    Eerror.size++;
  }
}

ErrorList *ErrorFind(ErrorList *list, uc16 errorcode)
{
  ErrorList *t = list;
  while (t != NULL)
  {
    if (t->code == errorcode)
    {
      return t;
    }
    else
      t = t->next;
  }
  return NULL;
}
