
#ifndef __T_LCDREFRESH_H_
#define __T_LCDREFRESH_H_

#include "stm32f4xx.h"

#define LCD_LEN  160
extern u8 LCD_Table[LCD_LEN];
extern u8 LCD_Index;

int rt_thread_lcd_refresh_init(void);

#endif
