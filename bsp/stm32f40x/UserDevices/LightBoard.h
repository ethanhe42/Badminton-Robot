/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIGHTBOARD_H
#define __LIGHTBOARD_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "MyTypeDef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/

extern u8 LB_Count(u8 port);
void LB_Init(void);
void LB_Test(void);
void LB_State(void);

#endif 

/**********************************END OF FILE*********************************/

