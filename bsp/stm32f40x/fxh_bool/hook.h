

#ifndef __HOOK_H_
#define __HOOK_H_
#include "rtthread.h"
void ttime_hook(struct rt_thread *from, struct rt_thread *to);
void ttime_reset(void);
void sys_ttime(void);
#endif

