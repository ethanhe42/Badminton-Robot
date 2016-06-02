#ifndef __LCDKEY_H
#define __LCDKEY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "MyTypeDef.h"

void LCD_SetXY(u8 x_index,u8 y_index);
void LCD_Clear(void);
void LCD_WriteString(char *str);
void LCD_Printf(const char *fmt,...);
void LCD_WriteInt(int val);
void LCD_WriteDouble(double val);

void Input_IntValue(int * address,char *name);
void Input_DoubleValue(double * address,char *name);
void Input_FloatValue(float * address,char *name);
void Input_List(struct Input_ListValue *list,int num);
void LCD_WriteNum(double data);

#endif 
