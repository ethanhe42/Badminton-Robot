
#ifndef __TIM_H__
#define __TIM_H__
#include "stm32f4xx.h"

extern void tim_init(void);
extern void reset_tim(u8 timx);
extern u32  get_tim(u8 timx);
extern void tim_delay(u8 timx, u32 us);

#endif
