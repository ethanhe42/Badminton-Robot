
#include "includes.h"
#include "t_sensor.h"


ALIGN(RT_ALIGN_SIZE)
static char thread_sensor_stack[1024];
struct rt_thread thread_sensor;

static void rt_thread_entry_sensor(void* parameter)
{
    static rt_uint32_t key_value = 0;
    while(1)
    {
        if(rt_mb_recv(&Mb_SensorStart, &key_value, RT_WAITING_FOREVER) == RT_EOK)
        {
            switch(key_value)
            {
                case eLsld_start: ls1p(e1p_ld); break;
                case eLspw_start: ls1p(e1p_pw); break;
                default : break;
            }
        }
        Delay_ms(5);
    }
} 


int rt_thread_sensor_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_sensor, 
                            "thread_sensor",
                            rt_thread_entry_sensor,
                            RT_NULL,
                            &thread_sensor_stack[0], sizeof(thread_sensor_stack),
							16, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_sensor);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
