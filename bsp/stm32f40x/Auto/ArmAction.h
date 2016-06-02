/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ARMACTION_H
#define __ARMACTION_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

/* Exported macro ------------------------------------------------------------*/
#define CHILD_ON  (!SA_READ(3)||!SA_READ(4))
#define CHILD_OFF (SA_READ(3) && SA_READ(4))

#define WAIT 1
#define NO   0

/* Exported variables --------------------------------------------------------*/
extern float Motor1_Aim_Pos[];
extern float Motor2_Aim_Pos[];
extern float Motor3_Aim_Pos[];
/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/

void armaction1(void);
void armaction2(void);
void armaction3(void);
void armaction4(void);
void armaction5(void);
void armaction6(void);
void armaction7(void);

void HandFB(float rotation,u8 state,int16_t speed);
void HandUD(float rotation,u8 state);

#endif 

/**********************************END OF FILE*********************************/

