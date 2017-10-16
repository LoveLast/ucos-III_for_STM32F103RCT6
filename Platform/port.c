/**
  ******************************************************************************
  * ��ʼ������
	* @Copyright Han ZHANG(haldak) @ Wenhao DING(Gilgamesh) | All rights reserved.
	* �廪��ѧ����ϵӲ���� & δ��ͨ����Ȥ�Ŷ�
	* Ӳ����ƴ���
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "port.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
static __IO uint32_t uwTimingDelay;

/* Private function prototypes -----------------------------------------------*/
void RCC_init(void);
void GPIO_init(void);
void NVIC_init(void);
void EXTI_init(void);

void Systick_init(void);

/* Private functions ---------------------------------------------------------*/
//ϵͳʱ������
void RCC_init(void)
{
	ErrorStatus HSEStartUpStatus;
	
	/* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();
 
	/* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
		
		/* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		
		/* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08) {}
  }

	/* Enable Clock Security System(CSS): this will generate an NMI exception
     when HSE clock fails */
  RCC_ClockSecuritySystemCmd(ENABLE);
	
	RCC_GetClocksFreq(&RCC_Clocks);
	
	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* Enable GPIOs clocks */
	RCC_APB2PeriphClockCmd(
						RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD , ENABLE);
}

//GPIO��ʼ��
void GPIO_init(void)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
}

//NVIC - Ƕ�������жϿ���������
void NVIC_init(void)
{
	//NVIC_InitTypeDef NVIC_InitStructure; //NVIC - Ƕ�������жϿ�����	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//2+2ģʽ
}

//EXTI - �ⲿ�ж�����
void EXTI_init(void)
{
//	EXTI_InitTypeDef EXTI_InitStructure;
}

/* ���������� */
//Systick����
//������SysTick����Ŀ��Ĭ��Ӧ����1ms/Tick
void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

//ÿ��SysTick�����
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0)
  {  
    uwTimingDelay--;
  }
}

void Systick_init(void)
{
	//Set NVIC for SysTick
	NVIC_SetPriority (SysTick_IRQn, 0x05); //��ռ���ȼ�1����Ӧ���ȼ�1
	
	/* SysTick end of count event each 1ms */
	//RCC_GetClocksFreq(&RCC_Clocks); //ϵͳʱ���Ѿ���ʱ���������ȡ��ȫ�ֱ���
	if (SysTick_Config(RCC_Clocks.HCLK_Frequency / CLOCKS_PER_SEC))
	{
		/* Capture error */
		while (1);
	}	
}

//�����ʼ��
void Periph_init()
{
	Systick_init();
}

//ϵͳ�ܳ�ʼ��
void Our_Sys_Init(void)
{
	RCC_init();
	GPIO_init();
	NVIC_init();
	EXTI_init();
	Periph_init();
}
