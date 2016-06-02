/*
 * File      : usart.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __USART_H__
#define __USART_H__

#include <rthw.h>
#include <rtthread.h>

void Com_Printf(u8 uartx, char *fmt, ...);
void Uarts_Init(void);
char * itob(u32 val);
void Com_SendByte(u8 uartx, u8 byte);
//s32 LaserRead(u8 laserx);
#endif
