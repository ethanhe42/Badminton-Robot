
#ifndef __T_SENSOR_H_
#define __T_SENSOR_H_

#include "stm32f4xx.h"
extern struct rt_thread thread_sensor;
int rt_thread_sensor_init(void);

typedef enum
{
    eLsld_start = 1,
    eLsld_find ,
    eLsld_end ,
    eLspw_start ,
    eLspw_find ,
    eLspw_end ,
}eSensor;

#endif
