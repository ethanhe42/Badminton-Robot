/**
  ******************************************************************************
  * @author  fxh_bool
  * @date    2013-
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IO_H
#define __IO_H

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void LED_On(u8 ledx);
void LED_Off(u8 ledx);
void LED_Toggle(u8 ledx);
void LED_Twinkle(u8 ledx);
void IO_Init(void);
void Buz_On(void);
void Buz_Off(void);
void Buz_Toggle(void);

/* Exported function prototype -----------------------------------------------*/

#endif 

/**********************************END OF FILE*********************************/

