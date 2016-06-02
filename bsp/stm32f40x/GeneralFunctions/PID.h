/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PID_H
#define __PID_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "MyTypeDef.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

extern u8 PID_flag;

extern struct PID Pid_List[];

extern fp64 Vout[5]; 

/* Exported function prototype -----------------------------------------------*/

void PID_Clear(void);
double PID(double Error,struct PID pid);
double AnglePID(double Error,struct PID pid);
double DistancePID(double Error,struct PID pid);

extern fp64 Cam_X_PID(s16 error_dis,struct PID pid);
extern fp64 Cam_Y_PID(s16 error_dis,struct PID pid);
extern fp64 Cam_Z_PID(s16 error_dis,struct PID pid);
extern fp64 Cam_A_PID(double error_angle,struct PID pid);
extern fp64 WT_D1_PID(double error_dis,struct PID pid);
extern fp64 WT_D2_PID(double error_dis,struct PID pid);
#endif 

/**********************************END OF FILE*********************************/
