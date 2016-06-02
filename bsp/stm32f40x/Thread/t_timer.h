#ifndef __T_TIMRT_H
#define __T_TIMRT_H
#include "stm32f4xx.h"
void rt_time_init(void);
void sys_Timer(void);
void Init_Timer(void);
int rt_thread_tim_init(void);
void TIM3_Syn_Init(void);
extern struct rt_thread thread_time;
//extern struct TIME_State TIMER;
#endif
/********************END OF FILE************************/
