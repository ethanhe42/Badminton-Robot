#ifndef __T_REALSPE_H_
#define __T_REALSPE_H_
#define fp64 double
#include "stm32f4xx.h"


int rt_thread_my_GPS_init(void);
extern Gps_State Gps_List[2];

extern struct Pointfp64 my_GPS;
#endif
