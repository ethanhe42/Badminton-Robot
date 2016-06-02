// /* Define to prevent recursive inclusion -------------------------------------*/
// #ifndef __MECANUM_H
// #define __MECANUM_H

// /* Includes ------------------------------------------------------------------*/
// #include "stm32f4xx.h"
// #include "MyTypeDef.h"
// /* Exported macro ------------------------------------------------------------*/

// #define SPEED_STOP Speed_X = Speed_Y = Speed_Rotation = 0

// /* Exported variables --------------------------------------------------------*/

// extern double Speed_X;
// extern double Speed_Y;
// extern double Speed_Rotation;

// extern struct Mecanum_State Mec_FL;		//四轮的状态参数
// extern struct Mecanum_State Mec_FR;
// extern struct Mecanum_State Mec_BL;
// extern struct Mecanum_State Mec_BR;

// /* Exported function prototype -----------------------------------------------*/
// void Mecanum_test(struct Mecanum_State *mec);
// //void SetSpeed(double fl_speed,double fr_speed,double bl_speed,double br_speed);
// //void SetSpeed(void);
// void SetSpeed(double speed_x,double speed_y,double speed_rotation);
// #endif 

// /**********************************END OF FILE*********************************/
