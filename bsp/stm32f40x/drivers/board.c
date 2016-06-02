/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>
#include "stm32f4xx.h"
#include "board.h"
#include "includes.h"
/**
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*******************************************************************************
 * Function Name  : Delay_ x ms
 *******************************************************************************/
void Delay_ms(u32 delay)
{
	u32 ticks;
	ticks = RT_TICK_PER_SECOND*delay/1000;
	rt_thread_delay(ticks);
}


/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	rt_uint32_t         cnts;

	RCC_GetClocksFreq(&rcc_clocks);

	cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;
	cnts = cnts / 8;

	SysTick_Config(cnts);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

/**
 * This is the timer interrupt service routine.
 *
 */
extern int rt_count_read(void);
extern void rt_count_reset(void);
//extern struct camera_data5;

u8 Motor2_Time_Flag = 0;

void SysTick_Handler(void)
{
    static rt_uint32_t tim;
	/* enter interrupt */
	rt_interrupt_enter();
    
	rt_tick_increase();
    if(tim++ == RT_TICK_PER_SECOND/2)
    {
        tim = 0;
        LED_Toggle(1);
    }
	if(rt_count_read())
	{
		Buz_Off();
		rt_count_reset();
	}
	else
		Buz_On();
	
	if(Motor2_Time_Flag<20)
	{
		Motor2_Time_Flag++;
	}
	
    ttime_reset();
    
	camera_data1.time_flag++;
	camera_data2.time_flag++;
	camera_data3.time_flag++;
	camera_data4.time_flag++;
	camera_data5.time_flag++;
	
	/* leave interrupt */
	rt_interrupt_leave();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
// 	extern System_Clock_Init();
// 	/*这是我们加的，RTT没有配置时钟*/
// 	System_Clock_Init();
	
	
	NVIC_Configuration();		// NVIC Configuration
	SysTick_Configuration();	// Configure the SysTick
    Uarts_Init();				//
    PWM_Init();					//
    IO_Init();					//
    CANx_Init();				//
    tim_init();                 //added by fxh_bool 2014-5-5
//	FatFsInit();				//文件系统初始化
	
#ifdef RT_USING_CONSOLE
	rt_console_set_device(CONSOLE_DEVICE);
#endif
}

/*@}*/
