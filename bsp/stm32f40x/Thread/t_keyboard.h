
#ifndef __T_KEYBOARD_H_
#define __T_KEYBOARD_H_

#include "stm32f4xx.h"
#include "rtdef.h"

void rt_keyboard_init( void );

extern struct rt_thread thread_keyboard;

#endif
