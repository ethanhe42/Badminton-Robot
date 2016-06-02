/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GOLINE_H
#define __GOLINE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "MyTypeDef.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void SetLine(struct Point end_point,double aim_radian,double speed_sta,double speed_max,double speed_end);
void GoLine(void);
#endif 

/**********************************END OF FILE*********************************/
