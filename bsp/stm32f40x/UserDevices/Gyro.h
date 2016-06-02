/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GYRO_H
#define __GYRO_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "MyTypeDef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

extern struct Gyro_State Gyro;

/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void Gyro_Init(void);
void Gyro_Set(void);
void Gyro_CheckFloat(void);



#endif 

/**********************************END OF FILE*********************************/

