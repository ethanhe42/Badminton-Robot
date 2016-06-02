
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* 邮箱控制块 */
struct rt_mailbox Mb_Key;
struct rt_mailbox Mb_Emergency;
struct rt_mailbox Mb_Auto;
struct rt_mailbox Mb_Arm;
struct rt_mailbox Mb_SensorStart;
struct rt_mailbox Mb_SensorEnd;

/* 用于放邮件的内存池 */
static char Mb_Key_pool[128];
static char Mb_Emergency_pool[128];
static char Mb_Auto_pool[128];
static char Mb_Arm_pool[128];
static char Mb_SensorStart_pool[128];
static char Mb_SensorEnd_pool[128];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
 * 函数名: Mb_Key_Init
 * 描  述: 按键邮箱初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int Mb_Key_Init(void)
{
    rt_err_t result;
    
    result = rt_mb_init(
						&Mb_Key,
						"Mb_Key",             			// 名称
						Mb_Key_pool,       				// 邮箱用到的内存池
						sizeof(Mb_Key_pool)/4, 			// 邮箱中的邮件数目，一封邮件占4字节
						RT_IPC_FLAG_FIFO); 				// 采用FIFO方式进行线程等待
    
    if (result != RT_EOK)
    {
        rt_kprintf("init Mb_Key failed.\n");
        return 0;
	}
    return 1;
}

/*
 * 函数名: Mb_Emergency_Init
 * 描  述: 紧急处理邮箱初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int Mb_Emergency_Init(void)
{
    rt_err_t result;
    
    result = rt_mb_init(
						&Mb_Emergency,
						"Mb_Emergency",             	// 名称
						Mb_Emergency_pool,       		// 邮箱用到的内存池
						sizeof(Mb_Emergency_pool)/4, 	// 邮箱中的邮件数目，一封邮件占4字节
						RT_IPC_FLAG_FIFO); 				// 采用FIFO方式进行线程等待
    
    if (result != RT_EOK)
    {
        rt_kprintf("init Mb_Emergency failed.\n");
        return 0;
	}
    return 1;
}

/*
 * 函数名: Mb_Auto_Init
 * 描  述: 自动任务邮箱初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int Mb_Auto_Init(void)
{
    rt_err_t result;
    
    result = rt_mb_init(
						&Mb_Auto,
						"Mb_Auto",             		// 名称
						Mb_Auto_pool,       		// 邮箱用到的内存池
						sizeof(Mb_Auto_pool)/4, 	// 邮箱中的邮件数目，一封邮件占4字节
						RT_IPC_FLAG_FIFO); 			// 采用FIFO方式进行线程等待
    
    if (result != RT_EOK)
    {
        rt_kprintf("init Mb_Auto failed.\n");
        return 0;
	}
    return 1;
}

/*
 * 函数名: Mb_Arm_Init
 * 描  述: 自动任务上层动作邮箱初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int Mb_Arm_Init(void)
{
    rt_err_t result;
    
    result = rt_mb_init(
						&Mb_Arm,
						"Mb_Arm",             		// 名称
						Mb_Arm_pool,       			// 邮箱用到的内存池
						sizeof(Mb_Arm_pool)/4, 		// 邮箱中的邮件数目，一封邮件占4字节
						RT_IPC_FLAG_FIFO); 			// 采用FIFO方式进行线程等待
    
    if (result != RT_EOK)
    {
        rt_kprintf("init Mb_Arm failed.\n");
        return 0;
	}
    return 1;
}

int Mb_SensorStart_Init(void)
{
    rt_err_t result;
    
    result = rt_mb_init(
						&Mb_SensorStart,
						"Mb_SensorStart",             		// 名称
						Mb_SensorStart_pool,       			// 邮箱用到的内存池
						sizeof(Mb_SensorStart_pool)/4, 		// 邮箱中的邮件数目，一封邮件占4字节
						RT_IPC_FLAG_FIFO); 			// 采用FIFO方式进行线程等待
    
    if (result != RT_EOK)
    {
        rt_kprintf("init Mb_SensorStart failed.\n");
        return 0;
	}
    return 1;
}

int Mb_SensorEnd_Init(void)
{
    rt_err_t result;
    
    result = rt_mb_init(
						&Mb_SensorEnd,
						"Mb_SensorEnd",             		// 名称
						Mb_SensorEnd_pool,       			// 邮箱用到的内存池
						sizeof(Mb_SensorEnd_pool)/4, 		// 邮箱中的邮件数目，一封邮件占4字节
						RT_IPC_FLAG_FIFO); 			        // 采用FIFO方式进行线程等待
    
    if (result != RT_EOK)
    {
        rt_kprintf("init Mb_SensorEnd failed.\n");
        return 0;
	}
    return 1;
}



/********************************    END OF FILE    ***************************/

