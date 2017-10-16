/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp.h"
#include "includes.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/* Tick timer count. */
void SysTick_Handler(void)
{
	OSIntEnter();
	OSTimeTick();
	OSIntExit();
}


extern OS_SEM Usart1_TXE_SEM;//用于表示USart1 Tx为空的信号量
extern OS_SEM Usart1_RXNE_SEM;//用于表示Usart1 Rx为空的信号量
OS_ERR usart_err;
void USART1_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART1,USART_IT_TXE)!=RESET){
		
		OSSemPost(&Usart1_TXE_SEM,
							OS_OPT_PEND_ABORT_1,
							&usart_err);
		USART_ClearITPendingBit(USART1,USART_IT_TXE);
		USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
	}
	
	else if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		OSSemPost(&Usart1_RXNE_SEM,
							OS_OPT_PEND_ABORT_1,
							&usart_err);
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	}
}

extern OS_FLAG_GRP I2CEventFlagGrp;
OS_ERR I2C_err;
void I2C1_EV_IRQHandler(void)
{
	switch(I2C_GetLastEvent(I2C1))
	{
		case I2C_EVENT_MASTER_MODE_SELECT:                  //EV5
			OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_MODE_SELECT,
								 (OS_OPT)OS_OPT_POST_FLAG_SET,
								 &I2C_err);
			break;
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:    //EV6
			OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,
								 (OS_OPT)OS_OPT_POST_FLAG_SET,
								 &I2C_err);
			break;
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:            //EV8
			OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_BYTE_TRANSMITTING,
								 (OS_OPT)OS_OPT_POST_FLAG_SET,
								 &I2C_err);
			break;
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:             //EV8-2
			OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_BYTE_TRANSMITTED,
								 (OS_OPT)OS_OPT_POST_FLAG_SET,
								 &I2C_err);
			break;
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:       //EV6
			OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,
								 (OS_OPT)OS_OPT_POST_FLAG_SET,
								 &I2C_err);
			break;
		case I2C_EVENT_MASTER_BYTE_RECEIVED:                //EV7
			OSFlagPost(&I2CEventFlagGrp,
								 I2C_EVENT_MASTER_BYTE_RECEIVED,
								 (OS_OPT)OS_OPT_POST_FLAG_SET,
								 &I2C_err);
			break;
		case I2C_FLAG_TXE:
			
			
			OSFlagPost(&I2CEventFlagGrp,
								 I2C_FLAG_TXE,
								 (OS_OPT)OS_OPT_POST_FLAG_SET,
								 &I2C_err);
		
			
			
			I2C_ITConfig(I2C1,I2C_IT_BUF,DISABLE);
			break;
		default:
			break;
	}
	I2C_ITConfig(I2C1,I2C_IT_EVT,DISABLE);
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
