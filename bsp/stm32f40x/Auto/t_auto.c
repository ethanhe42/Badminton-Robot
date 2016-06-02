
#include "includes.h"

ALIGN(RT_ALIGN_SIZE)
static char thread_auto_stack[1024];
struct rt_thread thread_auto;

/*
 * 函数名: rt_thread_entry_auto
 * 描  述: 自动任务的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 */
static void rt_thread_entry_auto(void* parameter)
{
    rt_uint32_t msg;
	
    while(1)
    {
			if(rt_mb_recv(&Mb_Auto, &msg, RT_WAITING_FOREVER) == RT_EOK)
			{
			switch(msg)
			{
				case 1:
					PID_Clear();
				  MotorLED_Off();
					GoRoute1();
					break;
				case 2:
					PID_Clear();
					MotorLED_Off();
					GoRoute2();
					break;
				case 3:
					PID_Clear();
					MotorLED_Off();
					GoRoute3();
					break;

				case 4:
					PID_Clear();
					MotorLED_Off();
				  CamLED_Off();
					GoRoute4();
					break;
				
				case 5:
					PID_Clear();
					MotorLED_Off();
					CamLED_Off();
					GoRoute5();
					break;
				
				case 6:
					PID_Clear();
					MotorLED_Off();
					CamLED_Off();
					GoRoute6();
					break;
				
				case 7:
					PID_Clear();
					MotorLED_Off();
					CamLED_Off();
					GoRoute7();
				  break;
				
				default :
				break;
			}
		}
    }
}

/*sy
 * 函数名: rt_thread_auto_init
 * 描  述: 自动任务线程的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_auto_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_auto, 
                            "thread_auto",
                            rt_thread_entry_auto,
                            RT_NULL,
                            &thread_auto_stack[0], sizeof(thread_auto_stack),
							9, 5);
    if (result == RT_EOK)
        rt_thread_startup(&thread_auto);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
