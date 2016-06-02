
#ifndef __T_REALSPE_H_
#define __T_REALSPE_H_

#include "stm32f4xx.h"

extern double realspe,realspe_y,realspe_x;
int rt_thread_realspe_init(void);
extern double speed[5][2];
extern double yiqi[5][2];
#endif
