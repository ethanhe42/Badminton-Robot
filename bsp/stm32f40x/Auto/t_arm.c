#include "includes.h"

ALIGN(RT_ALIGN_SIZE)
static char thread_arm_stack[2048];
struct rt_thread thread_arm;


/*
 * 函数名: rt_thread_entry_arm
 * 描  述: 自动任务上层动作的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 */
static void rt_thread_entry_arm(void* parameter)
{
    rt_uint32_t msg;

    while (1)
    {
			if(rt_mb_recv(&Mb_Arm, &msg, RT_WAITING_FOREVER) == RT_EOK)
			{
			switch(msg)
			{
				case 1:
					armaction1();
					break;
				case 2:
					armaction2();
					break;
				case 3:
					armaction3();
					break;
				case 4:
					armaction4();
					break;
				case 5:
					armaction5();
					break;
				case 6:
					armaction6();
					break;
				case 7:
					armaction7();
					break;
			}
		}
    }
}

/*
 * 函数名: rt_thread_arm_init
 * 描  述: 自动任务上层动作的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_arm_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_arm, 
                            "thread_arm",
                            rt_thread_entry_arm,
                            RT_NULL,
                            &thread_arm_stack[0], sizeof(thread_arm_stack),
							8, 5);
    if (result == RT_EOK)
        rt_thread_startup(&thread_arm);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
