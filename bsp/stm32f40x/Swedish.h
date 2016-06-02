/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SWEDISH_H
#define __SWEDISH_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "MyTypeDef.h"
/* Exported macro ------------------------------------------------------------*/
#define SPEED_STOP Speed_X = Speed_Y = Speed_Rotation = 0

/* Exported variables --------------------------------------------------------*/

extern double Speed_X;
extern double Speed_Y;
extern double Speed_Rotation;

extern struct Swedish_State Swe_FL;		//四轮的状态参数
extern struct Swedish_State Swe_FR;
extern struct Swedish_State Swe_BL;
extern struct Swedish_State Swe_BR;

/* Exported function prototype -----------------------------------------------*/
void Swedish_test(struct Swedish_State *swe);
//void SetSpeed(double fl_speed,double fr_speed,double bl_speed,double br_speed);
//void SetSpeed(void);
void SetSpeed(double speed_x,double speed_y,double speed_rotation);
#endif 

/**********************************END OF FILE*********************************/
