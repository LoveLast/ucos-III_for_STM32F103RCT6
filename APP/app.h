#ifndef __APP_H
#define __APP_H
#include "includes.h"
void Task_Start(void *p_arg);
void Task_LED1(void *p_arg);
void Task_LED2(void *p_arg);
void Task_LED3(void *p_arg);
void Usart1_SendMsgTask(void *p_arg);
void Usart1_StoreMsgTask(void *p_arg);
void Usart1_SendStr(char *str,unsigned short int sz);
void I2C1_SendByte(unsigned char addr,unsigned char byte);
CPU_INT08U* Usart1_ReceiveData(CPU_INT08U StaTest,CPU_INT08U EndTest);
void Usart1_RxDataDealTask(void *p_arg);
#endif
