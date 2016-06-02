/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPS_H
#define __GPS_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "MyTypeDef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

extern struct GPS_State GPS;

/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void GPS_Init(struct Point position,double radian);
void GPS_Clear(void);


#endif 

/**********************************END OF FILE*********************************/

