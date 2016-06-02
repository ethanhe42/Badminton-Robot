
#include "includes.h"

#include "M_List.h"
#include "M_Func.h"

#include "t_keyboard.h"
#include "T_Auto.h"
#include "T_Arm.h"


ALIGN(RT_ALIGN_SIZE)
static char thread_emergency_stack[1024];
struct rt_thread thread_emergency;

/*
 * 函数名: rt_thread_entry_emergency
 * 描  述: 紧急处理线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 */
extern u8 armtest_flag;
extern u8 WalkGround;
extern u8 Retry;

static void rt_thread_entry_emergency(void* parameter)
{
	rt_uint32_t msg;
	rt_uint32_t level;
	
    while(1)
    {
		if(rt_mb_recv(&Mb_Emergency, &msg, RT_WAITING_FOREVER) == RT_EOK)
		{
			level = rt_hw_interrupt_disable();		//关中断
			
			rt_mb_detach(&Mb_Arm);
			rt_mb_detach(&Mb_Auto);
			Mb_Arm_Init();
			Mb_Auto_Init();
			
			//Retry时如果把手柄线程给删掉，那后面的命令就执行不了了，所以Retry时不能删。
			//同样在Retry时如果把按键线程也给删掉，那么一方面，计时会结束，另一方面该线程会一直等待按键。那么别的等待按键就失灵了
			if(msg == 6)
			{
				rt_thread_detach(&thread_keyboard);		//按键线程脱离
				rt_keyboard_init();						//按键线程重新初始化
				rt_thread_detach(&thread_handle);		//手柄线程脱离
				rt_thread_handle_init();				//手柄线程重新初始化
			}
			else if(!WalkGround && !Retry)
			{
				rt_thread_detach(&thread_keyboard);		//按键线程脱离
				rt_keyboard_init();						//按键线程重新初始化
				rt_thread_detach(&thread_handle);		//手柄线程脱离
				rt_thread_handle_init();				//手柄线程重新初始化
			}
			rt_thread_detach(&thread_auto);
			rt_thread_auto_init();
			rt_thread_detach(&thread_arm);
			rt_thread_arm_init();
			
			SPEED_STOP;
			Moto_Stop(W_MOTOR1_OLD_ID);
			Moto_Stop(W_MOTOR2_OLD_ID);
			Moto_Stop(W_MOTOR3_OLD_ID);
			
			armtest_flag=1;
// 			RouteFinish=1;
			
			CurrentMenu = MainMenu;					//设置当前显示页面为主菜单
			ShowMenuList();
			
			rt_hw_interrupt_enable(level);			//开中断
		}
    }
}

/*
 * 函数名: rt_thread_emergency_init
 * 描  述: 紧急处理线程的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_emergency_init(void)//紧急处理线程初始化
{
    rt_err_t result;
    
	result = rt_thread_init(
							&thread_emergency, 
                            "thread_emergency",
                            rt_thread_entry_emergency,
                            RT_NULL,
                            &thread_emergency_stack[0], sizeof(thread_emergency_stack),
							4, 5);
    if (result == RT_EOK)
        rt_thread_startup(&thread_emergency);
    else
        return RT_ERROR;

    return RT_EOK;
}
