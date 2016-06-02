#include "includes.h"
#include "t_realspe.h"
#include "t_handle.h"
#include "t_realspe.h"
#include "math.h"
static char thread_realspe_stack[1024];
struct rt_thread thread_realspe;
double realspe,realspe_x,realspe_y;//really_speed//
double yiqi[5][2]={0};
double speed[5][2]={0};
/*
 * 函数名: rt_thread_entry_realspe
 * 描  述: 算实际速度线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 */
static void rt_thread_entry_realspe(void* parameter)
{
  int n;
	
	while(1)
	{
//				LCD_SetXY(0,5);
//        LCD_WriteString("6.realspe_y:");
//        LCD_WriteDouble(realspe_y);
//		
//        LCD_SetXY(0,6);
//        LCD_WriteString("6.realspe:");
//        LCD_WriteDouble(realspe);

//				LCD_SetXY(0,7);
//        LCD_WriteString("Handle_Y:");
//        LCD_WriteDouble(Handle_Speed_Y);

		Delay_ms(5);
		n=n%5;	

			//获取坐标
 		yiqi[n][0]=MISO_GPS_X.f64_data;
		yiqi[n][1]=MISO_GPS_Y.f64_data;  
			
		if(n!=0)
		{
			speed[n][0]=sqrt(pow((yiqi[n-1][0]-yiqi[n][0]),2))/0.005;
			speed[n][1]=sqrt(pow((yiqi[n-1][1]-yiqi[n][1]),2))/0.005;
		}
		else
		{
			speed[n][0]=sqrt(pow((yiqi[4][0]-yiqi[n][0]),2))/0.005;
			speed[n][1]=sqrt(pow((yiqi[4][1]-yiqi[n][1]),2))/0.005;
		}	
			//通过坐标算得实际速度
			
		if(n==0)
		{
			realspe_x=(speed[4][0]+speed[3][0]+speed[2][0]+speed[0][0])/4;
			realspe_y=(speed[4][1]+speed[3][1]+speed[2][1]+speed[0][1])/4;
			realspe=sqrt(pow(realspe_x,2)+pow(realspe_y,2));
		}
		else if(n==1)
		{
			realspe_x=(speed[4][0]+speed[3][0]+speed[1][0]+speed[0][0])/4;
			realspe_y=(speed[4][1]+speed[3][1]+speed[1][1]+speed[0][1])/4;
			realspe=sqrt(pow(realspe_x,2)+pow(realspe_y,2));
		}
		else if(n==2)
		{
			realspe_x=(speed[4][0]+speed[1][0]+speed[2][0]+speed[0][0])/4;
			realspe_y=(speed[4][1]+speed[1][1]+speed[2][1]+speed[0][1])/4;
			realspe=sqrt(pow(realspe_x,2)+pow(realspe_y,2));
		}
		else if(n==3)
		{
			realspe_x=(speed[1][0]+speed[3][0]+speed[2][0]+speed[0][0])/4;
			realspe_y=(speed[1][1]+speed[3][1]+speed[2][1]+speed[0][1])/4;
			realspe=sqrt(pow(realspe_x,2)+pow(realspe_y,2));
		}		
		else if(n==4)
		{
			realspe_x=(speed[4][0]+speed[3][0]+speed[2][0]+speed[1][0])/4;
			realspe_y=(speed[4][1]+speed[3][1]+speed[2][1]+speed[1][1])/4;
			realspe=sqrt(pow(realspe_x,2)+pow(realspe_y,2));
		}	
		n++;
	}
} 

/*
 * 函数名: rt_thread_realspe_init
 * 描  述: realspe刷新线程的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_realspe_init(void)
{
	rt_err_t result;

    
	result = rt_thread_init(&thread_realspe, 
                            "thread_realspe",
                            rt_thread_entry_realspe,
                            RT_NULL,
                            &thread_realspe_stack[0], sizeof(thread_realspe_stack),
							5, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_realspe);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
