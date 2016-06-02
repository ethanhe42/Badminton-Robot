/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_H
#define __ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "MyTypeDef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

extern struct Encoder_State Encoder[];

/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void Encoder_Init_Con(void);
void Encoder_Init_R(void);
void Encoder_View(void);
void Encoder_Set(void);



#endif 

/**********************************END OF FILE*********************************/

