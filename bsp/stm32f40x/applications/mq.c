
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/*消息控制块*/
struct rt_messagequeue mq_sd;



/*用于存放消息的内存池 */
static char msg_pool[128*64];



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
 * 函数名: Mq_Sd_Init
 * 描  述: SD卡消息队列初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int Mq_Sd_Init(void)
{
    rt_err_t result;
    
	rt_mq_init(&mq_sd,
			   "mq_sd",                   //名称
			   &msg_pool[0],			  //消息队列用到的内存池
			   128,						  //消息队列中的消息大小
			   sizeof(msg_pool),		  //消息队列大小
			   RT_IPC_FLAG_FIFO);		  //采用FIFO方式进行线程等待
    
    if (result != RT_EOK)
    {
        rt_kprintf("init Mq_Sd failed.\n");
        return 0;
	}
    return 1;
}

/********************************    END OF FILE    ***************************/

