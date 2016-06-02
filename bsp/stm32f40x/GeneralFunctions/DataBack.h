/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATABACK_H
#define __DATABACK_H


#define DataBackEnable  0	//数据回传功能使能

#if DataBackEnalbe

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
void DataBack_Write(double data);
void DataBack_Send(void);
void DataBack_Init(void);
void DataBack_Save(void);
int DataBack_GetCount(void);
#endif 

#endif
/**********************************END OF FILE*********************************/
