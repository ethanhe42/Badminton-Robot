/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYSTRING_H
#define __MYSTRING_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
char *Int2Str(int IntData);
char *Float2Str(float FloatData, int PointNum);
char *GetSimpFile(char *str);


#endif 

/**********************************END OF FILE*********************************/
