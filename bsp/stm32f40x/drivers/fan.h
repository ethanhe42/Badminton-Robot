/**
  ******************************************************************************
  * @author  wrd_double
  * @date    2013-
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FAN_H
#define __FAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/



void Fan_Set_Max(void);
void Fan_Set_Min(void);
int Fan_Duty_Up(double);
int Fan_Duty_Dn(double);
void Fan_Stop(void);
void Fan_Duty(u8, double);
double get_fan_duty(u8 pwm_chx);
void Ductedfan_test(int *CH);
void FanAccelerate(double speed_x,double speed_y);

/* Exported function prototype -----------------------------------------------*/

#endif 

/**********************************END OF FILE*********************************/

