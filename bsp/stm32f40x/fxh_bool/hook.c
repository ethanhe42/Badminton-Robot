
#include "hook.h"
#include "rtthread.h"
#include "tim.h"
#include "sd_save.h"
#include "fxh_bool.h"
#include "includes.h"

#define USING_TTIME 1
#define TTIME   2   /* thread timer */

typedef struct
{
    int prio1;
    int prio2;
    int us;
}ttim_t;   /* thread timer type */

static ttim_t tims[25]; /*  */
const static int nsch=sizeof(tims)/sizeof(tims[0]);
static int sch_index = 0;

void ttime_hook(struct rt_thread *from, struct rt_thread *to)
{
    tims[nsch].prio1 = from->init_priority;
    tims[nsch].prio2 =   to->init_priority;
    tims[nsch].us    = get_tim(TTIME);
    sch_index ++;
}

void ttime_reset(void)
{
    reset_tim(TTIME);
    sch_index = 0;
}

void ttime_prt(void)    /* thread time print */
{   
//     int i=0;
//     for(i=0; i<nsch; i++)
//       SD_Printf_MQ();
}

void sys_ttime(void)
{
    int i=0;
    static rt_uint32_t key_value;
    
    while( 1 )
    {
        LCD_Clear();
        for(i=0; i<8; i++)
        {
            LCD_SetXY(0, i);    
            LCD_Printf("prio:%d time:%d", tims[i].prio1, tims[i].us);
        }
        
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            switch(key_value)
            {
                case key1:                  break;
                case key2:                  break;
                case keyback:               return;
            }
        }
        Delay_ms(15);
    }    

}

/* end of file */
