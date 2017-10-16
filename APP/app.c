
//Write your task hear

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define maxstring 10
#define maxlength 20

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
OS_TCB StartUp_TCB;
CPU_STK StartUp_Stk[80];

static OS_TCB LED1_TCB;
static CPU_STK LED1_Stk[128];

static OS_TCB LED2_TCB;
static CPU_STK LED2_Stk[128];

static OS_TCB LED3_TCB;
static CPU_STK LED3_Stk[128];

OS_TCB Usart1_StoreMsgTask_TCB;
CPU_STK Usart1_StoreMsgTask_Stk[512];

OS_TCB Usart1_SendMsg_TCB;
CPU_STK Usart1_SendMsg_Stk[256];

OS_TCB Usart1_RxDataDealTask_TCB;
CPU_STK Usart1_RxDataDealTask_Stk[512];

OS_SEM Usart1_TXE_SEM;//用于表示USart1 Tx为空的信号量
OS_SEM Storage_Left;//用于计数存储分区剩余空间

OS_Q Usart1_Str_Q;//用于存储将要被Usart1发送的字符串的消息队列
//OS_Q Usart1_Str_Len_Q;//用于存储将要被Usart1发送的字符串的长度的消息队列

OS_MEM Usart_Str_Mem;//存储即将要发送的字符串的存储分区
CPU_INT08U StringStorage[maxstring][maxlength];//最多存放maxstring个字符串，每个字符串长度最大为maxlength

OS_MEM Usart_Rx_Mem;//接收缓存区
CPU_INT08U Usart_Rx_Storage[maxstring][maxlength];//接收缓存区

OS_SEM Usart1_RXNE_SEM;//用于表示Usart1 Rx为空的信号量
OS_SEM Rx_Str_Left;//缓存区剩余空间

OS_FLAG_GRP I2CEventFlagGrp;//I2C事件标志组

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int main(void)
{
	//Start with board specific hardware init.
	OS_ERR err;
	
	
	
	
	OSInit(&err);
	
	OSSemCreate(&Usart1_TXE_SEM,
							"Usart1_TXE",
							0,
							&err);
	
	OSSemCreate(&Storage_Left,
							"Storage_Left",
							maxstring,
							&err);
	
	OSSemCreate(&Usart1_RXNE_SEM,
							"Usart1_RXNE",
							0,
							&err);
							
	OSSemCreate(&Rx_Str_Left,
							"Rx_Storage_Left",
							maxstring,
							&err);
	
	OSQCreate(&Usart1_Str_Q,
						"Usart1_Str",
						maxstring,
						&err);
	
	/*OSQCreate(&Usart1_Str_Len_Q,
						"Usart1_Str_Len",
						maxstring,
						&err);
						
						*/
	OSMemCreate(&Usart_Str_Mem,
							"Usart1_String",
							(void *)&StringStorage[0][0],
							maxstring,
							maxlength,
							&err);
	
	OSFlagCreate(&I2CEventFlagGrp,
							 "I2CEventFlagGrp",
							 (OS_FLAGS)0,
							 &err);
							
	OSTaskCreate((OS_TCB     *)&StartUp_TCB,
							 (CPU_CHAR   *)"Start",
							 (OS_TASK_PTR )Task_Start,
							 (void       *)0,
							 (OS_PRIO     )1,
							 (CPU_STK    *)&StartUp_Stk[0],
							 (CPU_STK_SIZE)8,
							 (CPU_STK_SIZE)80,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);
	
	OSStart(&err);
	//Infinite loop to deal with your project.
	/*while(1)
	{
		
	}*/
}

void Task_Start(void *p_arg)
{
	OS_ERR err;
	(void)p_arg;
	
	OSTaskCreate((OS_TCB     *)&LED1_TCB,
							 (CPU_CHAR   *)"LED1",
							 (OS_TASK_PTR )Task_LED1,
							 (void       *)0,
							 (OS_PRIO     )4,
							 (CPU_STK    *)&LED1_Stk[0],
							 (CPU_STK_SIZE)12,
							 (CPU_STK_SIZE)128,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);
							 
	OSTaskCreate((OS_TCB     *)&LED2_TCB,
							 (CPU_CHAR   *)"LED2",
							 (OS_TASK_PTR )Task_LED2,
							 (void       *)0,
							 (OS_PRIO     )5,
							 (CPU_STK    *)&LED2_Stk[0],
							 (CPU_STK_SIZE)12,
							 (CPU_STK_SIZE)128,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);
							 
	OSTaskCreate((OS_TCB     *)&LED3_TCB,
							 (CPU_CHAR   *)"LED3",
							 (OS_TASK_PTR )Task_LED3,
							 (void       *)0,
							 (OS_PRIO     )6,
							 (CPU_STK    *)&LED3_Stk[0],
							 (CPU_STK_SIZE)12,
							 (CPU_STK_SIZE)128,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);
							 
	OSTaskCreate((OS_TCB     *)&Usart1_SendMsg_TCB,
							 (CPU_CHAR   *)"Usart1_SendMsg_Task",
							 (OS_TASK_PTR )Usart1_SendMsgTask,
							 (void       *)0,
							 (OS_PRIO     )2,
							 (CPU_STK    *)&Usart1_SendMsg_Stk[0],
							 (CPU_STK_SIZE)25,
							 (CPU_STK_SIZE)256,
							 (OS_MSG_QTY  )0,
							 (OS_TICK     )0,
							 (void       *)0,
							 (OS_OPT      )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR),
							 (OS_ERR     *)&err);
							 
	OSTaskCreate(&Usart1_StoreMsgTask_TCB,
							 "Usart1_StoreMsgTask",
							 Usart1_StoreMsgTask,
							 (void *)0,
							 3,
							 &Usart1_StoreMsgTask_Stk[0],
							 51,
							 512,
							 maxstring,
							 0,
							 (void *)0,
							 OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 &err);
	
	OSTaskCreate(&Usart1_RxDataDealTask_TCB,
							 "Usart1_RxDataDealTask",
							 Usart1_RxDataDealTask,
							 (void *)0,
							 7,
							 &Usart1_RxDataDealTask_Stk[0],
							 51,
							 512,
							 maxstring,
							 0,
							 (void *)0,
							 OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 &err);
	Our_Sys_Init();
	OSTaskDel(&StartUp_TCB,&err);
}

void Task_LED1(void *p_arg)
{
	OS_ERR err;
	(void)p_arg;
	
	while(1)
	{
		Usart1_SendStr("haha",4);
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		
		
	}
		
}


void Task_LED2(void *p_arg)
{
	OS_ERR err;
	(void)p_arg;
	
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
		GPIO_SetBits(GPIOB,GPIO_Pin_14);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		
		
	}
		
}


void Task_LED3(void *p_arg)
{
	OS_ERR err;
	(void)p_arg;
	
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		GPIO_SetBits(GPIOB,GPIO_Pin_15);
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err);
		GPIO_ResetBits(GPIOB,GPIO_Pin_15);
		
		
	}
		
}

void Usart1_StoreMsgTask(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	CPU_INT08U *DataPtr;
	CPU_INT08U *str;
	OS_MSG_SIZE sz;
	
	while(1){
		str = (CPU_INT08U *)OSTaskQPend(0,
																	OS_OPT_PEND_BLOCKING,
																	&sz,
																	&ts,
																	&err);
		OSSemPend(&Storage_Left,
							0,
							OS_OPT_PEND_BLOCKING,
							&ts,
							&err);
		DataPtr = (CPU_INT08U *)OSMemGet(&Usart_Str_Mem,
																		&err);
	
		if(err == OS_ERR_NONE){
			if(sz<maxlength){
				strcpy((char *)DataPtr,(char *)str);
				OSQPost(&Usart1_Str_Q,
								(void *)DataPtr,
								(OS_MSG_SIZE)sz,
								(OS_OPT)OS_OPT_POST_FIFO,
								&err);
			}
			else{
				OSMemPut(&Usart_Str_Mem,
								(void *)DataPtr,
								&err);
			}
		}
	}
}

void Usart1_SendStr(char *str,unsigned short int sz)
{
	OS_ERR err;
	
	OSTaskQPost(&Usart1_StoreMsgTask_TCB,
							(void *)str,
							sz,
							OS_OPT_POST_FIFO,
							&err);
}

unsigned char *data;
OS_MSG_SIZE size;
void Usart1_SendMsgTask(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	unsigned char *temp;
	
	(void) p_arg;
	//完成一个字符串的发送
	while(1)
	{
		
		//等待消息队列中有需要发送的字符串
			data = (unsigned char *)OSQPend(&Usart1_Str_Q,
																			0,
																			OS_OPT_PEND_BLOCKING,
																			&size,
																			&ts,
																			&err
																			);
			temp=data;
			if(err==OS_ERR_NONE){
				
				while(size){
					//等待串口发送空闲
					USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
					OSSemPend(&Usart1_TXE_SEM,
							0,
							OS_OPT_PEND_BLOCKING,
							&ts,
							&err);
					if(err==OS_ERR_NONE){
						//逐个字符发送到TX
						USART_SendData(USART1,*data);
						data++;
						size--;
					}
				}
				OSMemPut(&Usart_Str_Mem,
								(void *)temp,
								&err);
				OSSemPost(&Storage_Left,
									OS_OPT_POST_1,
									&err);
			}
		
	}
}

//通过字符串的头尾协议进行接收
CPU_INT08U* Usart1_ReceiveData(CPU_INT08U StaTest,CPU_INT08U EndTest)
{
	OS_ERR err;
	CPU_TS ts;
	
	CPU_INT08U RxData;
	CPU_INT08U *RxDataPtr;
	CPU_INT08U *Res;
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	OSSemPend(&Usart1_RXNE_SEM,
					  0,
						OS_OPT_PEND_BLOCKING,
						&ts,
						&err);
	RxData = USART_ReceiveData(USART1);
	if(RxData == StaTest){
		OSSemPend(&Rx_Str_Left,
							0,
							OS_OPT_PEND_BLOCKING,
							&ts,
							&err);
		RxDataPtr =OSMemGet(&Usart_Rx_Mem,
												&err);
		Res=RxDataPtr;
		*RxDataPtr=RxData;
		RxDataPtr++;
		while(*RxDataPtr!=EndTest){
			USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
			OSSemPend(&Usart1_RXNE_SEM,
					  0,
						OS_OPT_PEND_BLOCKING,
						&ts,
						&err);
			*RxDataPtr = USART_ReceiveData(USART1);
			RxDataPtr++;
		}
	}
	return Res;
}

void Usart1_RxDataDealTask(void *p_arg)
{
	OS_ERR err;
	CPU_TS ts;
	
	while(1)
	{
		//Process to deal with the data
		
		//Put back the memory to the Usart_Rx_Mem
		
		//Post the sem to show that there's one more space in the memory
		OSSemPost(&Rx_Str_Left,
							OS_OPT_POST_1,
							&err);
	}
}

//完成单个字节的传输
void I2C1_SendByte(unsigned char addr,unsigned char byte){
	CPU_TS ts;
	OS_ERR err;
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF,ENABLE);
	
	//等待总线空闲同时发送为空
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_FLAG_TXE,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err);
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_FLAG_TXE,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);
	I2C_GenerateSTART(I2C1,ENABLE);
	//等待EV5并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_MODE_SELECT,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_MODE_SELECT,
								 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
								 &err);
	I2C_Send7bitAddress(I2C1,addr,I2C_Direction_Transmitter);
	//等待EV6并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);					 
	I2C_SendData(I2C1,byte);
	//等待EV8并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_BYTE_TRANSMITTED,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_BYTE_TRANSMITTED,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);							 
	I2C_GenerateSTOP(I2C1,ENABLE);
}

//使用I2C1发送一个字符串
void I2C1_SendStr(unsigned char addr,unsigned char * str,unsigned short int length)
{
	CPU_TS ts;
	OS_ERR err;
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF,ENABLE);
	
	//等待总线空闲同时发送为空
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_FLAG_TXE,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err);
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_FLAG_TXE,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);
	I2C_GenerateSTART(I2C1,ENABLE);
	//等待EV5并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_MODE_SELECT,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_MODE_SELECT,
								 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
								 &err);
	I2C_Send7bitAddress(I2C1,addr,I2C_Direction_Transmitter);
	//等待EV6并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);					 
	while(length--)
	{
		I2C_SendData(I2C1,*str);
		str++;
		//等待EV8并清除
		I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
		OSFlagPend(&I2CEventFlagGrp,
							I2C_EVENT_MASTER_BYTE_TRANSMITTED,
							0,
							OS_OPT_PEND_FLAG_SET_ALL,
							&ts,
							&err
							);
		OSFlagPost(&I2CEventFlagGrp,
							 I2C_EVENT_MASTER_BYTE_TRANSMITTED,
							 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
							 &err);			
	}		
	I2C_GenerateSTOP(I2C1,ENABLE);
}

//使用I2C1接收一个字符串
void I2C1_ReceiveSte(unsigned char addr,unsigned char ReadAddr,unsigned char *Data,unsigned short int length)
{
	CPU_TS ts;
	OS_ERR err;
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF,ENABLE);
	
	//等待总线空闲同时发送为空
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_FLAG_TXE,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err);
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_FLAG_TXE,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);
	I2C_GenerateSTART(I2C1,ENABLE);
	//等待EV5并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_MODE_SELECT,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_MODE_SELECT,
				  	 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);
	I2C_Send7bitAddress(I2C1,addr,I2C_Direction_Transmitter);
	//等待EV6并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);					
	I2C_Cmd(I2C1,ENABLE);//重启I2C1以清空EV6
	I2C_SendData(I2C1,ReadAddr);
	//等待EV8并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_BYTE_TRANSMITTED,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_BYTE_TRANSMITTED,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);		
	
	I2C_GenerateSTART(I2C1,ENABLE);
	//等待EV5并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_MODE_SELECT,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_MODE_SELECT,
				  	 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);
	I2C_Send7bitAddress(I2C1,addr,I2C_Direction_Receiver);
	//等待EV6并清除
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	OSFlagPend(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,
						 0,
						 OS_OPT_PEND_FLAG_SET_ALL,
						 &ts,
						 &err
						 );
	OSFlagPost(&I2CEventFlagGrp,
						 I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,
						 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
						 &err);
	
	while(length)
	{
		if(length == 1)
		{
			I2C_AcknowledgeConfig(I2C1,DISABLE);
			
			I2C_GenerateSTOP(I2C1,ENABLE);
		}
		
		//检查EV7以得知是否获得了信息
		I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
		OSFlagPend(&I2CEventFlagGrp,
						   I2C_EVENT_MASTER_BYTE_RECEIVED,
						   0,
						   OS_OPT_PEND_FLAG_SET_ALL,
						   &ts,
						   &err
						  );
		OSFlagPost(&I2CEventFlagGrp,
						   I2C_EVENT_MASTER_BYTE_RECEIVED,
							 (OS_OPT)OS_OPT_POST_FLAG_CLR+OS_OPT_POST_NO_SCHED,
							 &err);
		*Data = I2C_ReceiveData(I2C1);
		Data++;
		length--;
	}
	I2C_AcknowledgeConfig(I2C1,ENABLE);
}

