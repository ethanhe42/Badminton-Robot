/**
  ******************************************************************************
  * @file    IO_Toggle/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
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
#include "stm32f4xx.h"
#include <rtthread.h>
#include "board.h"
#include "includes.h"
/** @addtogroup STM32F4_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup IO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
	
static unsigned char i = 0;	
static char buffer[10]={'\0'};
static int  receive = 0;
double Speed_YFromHandle2;
int AngleHit;
int AngleSwing;

void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
//void HardFault_Handler(void)
//{
//    // definition in libcpu/arm/cortex-m4/context_*.S
//}

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
//    // definition in libcpu/arm/cortex-m4/context_*.S
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//    // definition in boarc.c
//}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/*can总线接收时判断有点多，望大家见谅
由于can总线挂载设备较多，每块板子都有各自的程序，所有板子都不出
问题的概率较小。Can_Database数据库是外设与主控通信的公共信息，外
设与主控的数据库必须完全统一。否则，就会造成通信故障。在接收时，
进行判断可以快速发现两板之间的信息不统一问题。
*/
void CAN1_RX0_IRQHandler(void)
{
	uint8_t i;
	CanRxMsg   RxMessage;
	
	/** @led add by fxh_bool 3-25 20:00 */
	LED_Toggle(3);
	
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
		(*(Can_Database[HASH_TABLE[RxMessage.StdId]].Data_ptr+i)) = RxMessage.Data[i];
	Can_Database[HASH_TABLE[RxMessage.StdId]].MenuFunc();

}
void CAN1_RX1_IRQHandler(void)
{
	uint8_t i;
	CanRxMsg   RxMessage;
	
	/** @led add by fxh_bool 3-25 20:00 */
	LED_Toggle(3);
	
	CAN_Receive(CAN1, CAN_FIFO1, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
		(*(Can_Database[HASH_TABLE[RxMessage.StdId]].Data_ptr+i)) = RxMessage.Data[i];
	Can_Database[HASH_TABLE[RxMessage.StdId]].MenuFunc();

}
void CAN2_RX0_IRQHandler(void)
{
	uint8_t i;
	CanRxMsg   RxMessage;
	
	/** @led add by fxh_bool 3-25 20:00 */
	LED_Toggle(3);
	
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
		(*(Can_Database[HASH_TABLE[RxMessage.StdId]].Data_ptr+i)) = RxMessage.Data[i];
	Can_Database[HASH_TABLE[RxMessage.StdId]].MenuFunc();
}

void CAN2_RX1_IRQHandler(void)
{
	uint8_t i;
	CanRxMsg   RxMessage;
	
	/** @led add by fxh_bool 3-25 20:00 */
	LED_Toggle(3);
	
	CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
		(*(Can_Database[HASH_TABLE[RxMessage.StdId]].Data_ptr+i)) = RxMessage.Data[i];
	Can_Database[HASH_TABLE[RxMessage.StdId]].MenuFunc();
}


void USART1_IRQHandler(void)
{
    
#ifdef RT_USING_UART1
    extern struct rt_device uart1_device;
	extern void rt_hw_serial_isr(struct rt_device *device);
    /* enter interrupt */
    rt_interrupt_enter();

    rt_hw_serial_isr(&uart1_device);

    /* leave interrupt */
    rt_interrupt_leave();
#endif
}


void USART2_IRQHandler(void)// 按键中断
{
	rt_uint32_t KeyValue;
	
    //LED_Toggle(2);
    rt_interrupt_enter();
	
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        KeyValue = USART_ReceiveData(USART2);
    }
	if(KeyValue == 6)
	{
		rt_mb_send(&Mb_Emergency, KeyValue);
	}
    else
		rt_mb_send(&Mb_Key, KeyValue);
    
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);  
    rt_interrupt_leave();
}

void USART3_IRQHandler(void)
{
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		short2double(USART_ReceiveData(USART3));
}

//void UART4_IRQHandler(void)
//{
//	  unsigned char temp_trx;
//	  char temp[10] = {'\0'};
//	  int t;  
//    rt_interrupt_enter();	

//	  temp_trx = 0;	
//	
//	  if(USART_GetITStatus(UART4,USART_IT_RXNE)== SET)
//		{
//		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
//		temp_trx = USART_ReceiveData(UART4);	
//		  if(temp_trx != 'F')
//			{
//			receive = (temp_trx - '0') + receive * 10;
//			buffer[i++]=temp_trx;
//	    //USART_SendData(UART4,temp_trx);
//      while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
//			}
//			else
//			{	
//      receive-=1000;  //receive 原来是0~2000的一个数
//			Speed_YFromHandle2 = (double)receive * MAXSPEEDOFY;
//       
//			receive = 0;
//    	i = 0;
//			for(t = 0;t < 10; t++)
//      buffer[t] = '\0';				
//      }			
//    }
//	  rt_interrupt_leave();
//}

void UART4_IRQHandler(void)
{
#ifdef RACKET
	unsigned char temp_trx;
	char temp[10] = {'\0'};
	int t;  
	rt_interrupt_enter();	

	temp_trx = 0;	

	if(USART_GetITStatus(UART4,USART_IT_RXNE)== SET)
	{
	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
	temp_trx = USART_ReceiveData(UART4);
		if(temp_trx == 'D')
		{
		{	
			
		AngleSwing = receive-200;		 
		receive = 0;
		i = 0;
		for(t = 0;t < 10; t++)
		buffer[t] = '\0';	
			
		}	  
		}
		
		else if(temp_trx == 'F')
		{
		{	
			
		AngleHit = receive-200;		 
		receive = 0;
		i = 0;
		for(t = 0;t < 10; t++)
		buffer[t] = '\0';	
			
		}	         
		}
		
		else
		{
		receive = (temp_trx - '0') + receive * 10;
		buffer[i++]=temp_trx;
		USART_SendData(UART4,temp_trx);
		while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
		}		
	}
	rt_interrupt_leave();
#endif
	
#ifdef TWOHANDLE
	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
#endif
}
void UART5_IRQHandler(void)
{
    USART_ClearITPendingBit(UART5, USART_IT_RXNE);
}


void USART6_IRQHandler(void)
{
    rt_interrupt_enter();
}


void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
  SD_ProcessIRQSrc();
}

void DMA2_Stream3_IRQHandler(void)
{
  /* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
  SD_ProcessDMAIRQ();
}
/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
