
#include "includes.h"
#include "t_lcd_refresh.h"


ALIGN(RT_ALIGN_SIZE)
static char thread_lcd_refresh_stack[1024];
struct rt_thread thread_lcd_refresh;

u8 LCD_Table[LCD_LEN];
u8 LCD_Index;
/*
 * 函数名: rt_thread_entry_lcd_refresh
 * 描  述: lcd刷新线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 * 说  明: 液晶可写20列，8行;从0列开始，从0行开始
 */
static void rt_thread_entry_lcd_refresh(void* parameter)
{
    int i=0;
    
    while(1)
    {
        Com_SendByte (1, 0xff);//命令字头
        Delay_ms(5);
		
        Com_SendByte (1, 30*(i/2)+(i%2)*10);//显示位置，两列显示
        Delay_ms(5);
		
		Dma_Out(1, &LCD_Table[10*i],10);
        
        if(i==15)//一共16组，共8x(2x10)个数据
            i=0;
        else
            i++;
        
        Delay_ms(5);
    }
} 

/*
 * 函数名: rt_thread_lcd_refresh_init
 * 描  述: lcd刷新线程的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_lcd_refresh_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_lcd_refresh, 
                            "thread_lcd_refresh",
                            rt_thread_entry_lcd_refresh,
                            RT_NULL,
                            &thread_lcd_refresh_stack[0], sizeof(thread_lcd_refresh_stack),
							15, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_lcd_refresh);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
