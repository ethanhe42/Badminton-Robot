/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/
#include "includes.h"
#include <stdio.h>
#include <board.h>

#include "t_keyboard.h"
#include "t_lcd_refresh.h"
#include "t_emergency.h"
#include "t_timer.h"
#include "T_Auto.h"
#include "T_Arm.h"
#include "t_realspe.h"
#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32_eth.h"
#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif
void rt_init_thread_entry(void* parameter)
{
    /* LwIP Initialization */
#ifdef RT_USING_LWIP
    {
        extern void lwip_sys_init(void);

        /* register ethernetif device */
        eth_system_device_init();

        rt_hw_stm32_eth_init();
        /* re-init device driver */
        rt_device_init_all();

        /* init lwip system */
        lwip_sys_init();
        rt_kprintf("TCP/IP initialized!\n");
    }
#endif

//FS
		{
				
		}
//GUI
}

int rt_application_init()
{
    rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

/**********************以下为用户线程**********************/
	
	rt_keyboard_init();						//按键线程的初始化			优先级:6
	rt_thread_lcd_refresh_init();	        //lcd刷新线程初始化			优先级:15
	rt_thread_emergency_init();		        //紧急处理线程初始化		    优先级:4
	rt_thread_handle_init();			    //手柄线程初始化				优先级:7
	rt_thread_handle_Y_init();
#ifdef RACKET
	rt_thread_catch_init();         //接球初始化		优先级:9
#endif
	rt_thread_auto_init();				    //自动任务线程初始化		    优先级:9
	rt_thread_arm_init();					//上层动作线程初始化		    优先级:8
	rt_thread_FATfs_init();				    //文件系统的初始化			优先级:32-5=27
  rt_thread_test_init();                  //test 线程                     26
	rt_thread_tim_init();
	rt_thread_realspe_init();
    /*-------------------------- 邮箱 ---------------------------------------*/
	Mb_Key_Init();			//按键的邮箱
	Mb_Emergency_Init();	//紧急处理的邮箱
	Mb_Auto_Init();			//自动任务邮箱
	Mb_Arm_Init();			//自动任务上层动作邮箱	
	/*------------------------ 消息队列--------------------------------------*/
	Mq_Sd_Init();   //SD卡消息队列
    
    /*------------------------------------------------------------------------*/
    rt_scheduler_sethook(ttime_hook);
    return 0;
}

/*
 *控制文件打开的控制台命令
*/

void  Creat_NewFloder(const char * str)
{
		f_mkdir(str);
}

FINSH_FUNCTION_EXPORT(Creat_NewFloder , Creat A New Folder) ; 

/*@}*/

