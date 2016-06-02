/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GOROUTE_H
#define __GOROUTE_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include <includes.h>

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern u8 Route_Num;
extern u8 RouteFinish;
extern int Route[];
/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void GoRoute1(void);
void GoRoute2(void);
void GoRoute3(void);
void GoRoute4(void);
void GoRoute5(void);
void GoRoute6(void);
void GoRoute7(void);

#endif 

/**********************************END OF FILE*********************************/

