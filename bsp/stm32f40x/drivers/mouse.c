#include <stdio.h>
#include <math.h>
#include "includes.h"

#define RADIO 1
struct rt_thread thread_mouse;
static char thread_mouse_stack[1024];

static void rt_thread_entry_mouse(void* parameter)
{
	struct Point end_point;
	while(1)
	{
		char BYTE1,BYTE2,BYTE3,BYTE4;
		int move_x,move_y;
		if((BYTE1&0x08)==1)
		{
			move_x=(255-(int)(BYTE2))*-1;
		}
		else
		{	
			move_x=(int)(BYTE2);
		}
		if((BYTE1&0x16)==1)
		{
			move_y=(int)(BYTE3);
		}
		else
		{
			move_x=(255-(int)(BYTE3))*-1;
		}
		end_point.x=move_x;
		end_point.y=move_y;
    SetLine(end_point,0,500,3000,0);		
		Delay_ms(5);
	}
}


int rt_thread_mouse_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_mouse, 
                            "thread_mouse",
                            rt_thread_entry_mouse,
                            RT_NULL,
                            &thread_mouse_stack[0], sizeof(thread_mouse_stack),
							26, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_mouse);
    else
        return RT_ERROR;

    return RT_EOK;
}

