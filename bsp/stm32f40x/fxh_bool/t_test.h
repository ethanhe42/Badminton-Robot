
#ifndef __T_TEST_H_
#define __T_TEST_H_

#include "stm32f4xx.h"
extern int buz_debug;
int rt_thread_test_init(void);
void buz_msg(int on1, int on2, int on3, int dly);

#endif
