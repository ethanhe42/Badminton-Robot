
#include "includes.h"
#include "t_test.h"


ALIGN(RT_ALIGN_SIZE)
static char thread_test_stack[1024];
struct rt_thread thread_test;

int buz_debug = 0;
static int buz_delay = 150;
static int onCnt[3] = {0};

void buz_msg(int on1, int on2, int on3, int dly)
{
    onCnt[0] = on1;
    onCnt[1] = on2;
    onCnt[2] = on3;
    buz_delay = dly;
}
static void rt_thread_entry_test(void* parameter)
{
    //buz_msg(1, 2, 3, 200);
    while(1)
    {
        int i,j;
        for(i=0; i<3; i++)
        {
            for(j=0; j<onCnt[i]; j++)
            {
                Buz_On();
                Delay_ms(buz_delay);
            }   
            
            Buz_Off();
            Delay_ms(buz_delay);
        }
    }
} 


int rt_thread_test_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_test, 
                            "thread_test",
                            rt_thread_entry_test,
                            RT_NULL,
                            &thread_test_stack[0], sizeof(thread_test_stack),
							26, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_test);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
