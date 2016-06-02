/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENUFUNC_H
#define __MENUFUNC_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Defines ------------------------------------------------------------------*/
/* Exported variables -------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* function prototypes -------------------------------------------------------*/
void ShowMenuList(void);
void KeyNumOperate(u8 KeyValue);
void GoKeyNum(int KeyNum);

#endif 

/**********************************END OF FILE*********************************/

