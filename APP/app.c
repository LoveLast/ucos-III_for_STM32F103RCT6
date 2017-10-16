
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

OS_SEM Usart1_TXE_SEM;//���ڱ�ʾUSart1 TxΪ�յ��ź���
OS_SEM Storage_Left;//���ڼ����洢����ʣ��ռ�

OS_Q Usart1_Str_Q;//���ڴ洢��Ҫ��Usart1���͵��ַ�������Ϣ����
//OS_Q Usart1_Str_Len_Q;//���ڴ洢��Ҫ��Usart1���͵��ַ����ĳ��ȵ���Ϣ����

OS_MEM Usart_Str_Mem;//�洢����Ҫ���͵��ַ����Ĵ洢����
CPU_INT08U StringStorage[maxstring][maxlength];//�����maxstring���ַ�����ÿ���ַ����������Ϊmaxlength

OS_MEM Usart_Rx_Mem;//���ջ�����
CPU_INT08U Usart_Rx_Storage[maxstring][maxlength];//���ջ�����

OS_SEM Usart1_RXNE_SEM;//���ڱ�ʾUsart1 RxΪ�յ��ź���
OS_SEM Rx_Str_Left;//������ʣ��ռ�

OS_FLAG_GRP I2CEventFlagGrp;//I2C�¼���־��

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
	//���һ���ַ����ķ���
	while(1)
	{
		
		//�ȴ���Ϣ����������Ҫ���͵��ַ���
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
					//�ȴ����ڷ��Ϳ���
					USART_ITConfig(USART1,USART_IT_TXE,ENABLE);
					OSSemPend(&Usart1_TXE_SEM,
							0,
							OS_OPT_PEND_BLOCKING,
							&ts,
							&err);
					if(err==OS_ERR_NONE){
						//����ַ����͵�TX
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

//ͨ���ַ�����ͷβЭ����н���
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

//��ɵ����ֽڵĴ���
void I2C1_SendByte(unsigned char addr,unsigned char byte){
	CPU_TS ts;
	OS_ERR err;
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF,ENABLE);
	
	//�ȴ����߿���ͬʱ����Ϊ��
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
	//�ȴ�EV5�����
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
	//�ȴ�EV6�����
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
	//�ȴ�EV8�����
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

//ʹ��I2C1����һ���ַ���
void I2C1_SendStr(unsigned char addr,unsigned char * str,unsigned short int length)
{
	CPU_TS ts;
	OS_ERR err;
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF,ENABLE);
	
	//�ȴ����߿���ͬʱ����Ϊ��
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
	//�ȴ�EV5�����
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
	//�ȴ�EV6�����
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
		//�ȴ�EV8�����
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

//ʹ��I2C1����һ���ַ���
void I2C1_ReceiveSte(unsigned char addr,unsigned char ReadAddr,unsigned char *Data,unsigned short int length)
{
	CPU_TS ts;
	OS_ERR err;
	I2C_ITConfig(I2C1,I2C_IT_EVT,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF,ENABLE);
	
	//�ȴ����߿���ͬʱ����Ϊ��
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
	//�ȴ�EV5�����
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
	//�ȴ�EV6�����
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
	I2C_Cmd(I2C1,ENABLE);//����I2C1�����EV6
	I2C_SendData(I2C1,ReadAddr);
	//�ȴ�EV8�����
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
	//�ȴ�EV5�����
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
	//�ȴ�EV6�����
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
		
		//���EV7�Ե�֪�Ƿ�������Ϣ
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

