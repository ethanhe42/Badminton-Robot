
#ifndef __T_FSYS_H_
#define __T_FSYS_H_

#include "includes.h"
#include "stm32f4xx.h"
#include "ff.h"
void SD_ReInit(void);
void SD_Walkground(void);
int rt_thread_FATfs_init(void);
extern FIL fil_pid;
#endif
