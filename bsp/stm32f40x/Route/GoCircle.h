/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GOCIRCLE_H
#define __GOCIRCLE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "MyTypeDef.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void SetCircle(struct Point center,double r,double speed);
void GoCircle(void);

#endif 

/**********************************END OF FILE*********************************/
