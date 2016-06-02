/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEEP_H
#define __KEEP_H

#define Aim_ss    0
#define Aim_pw    1
#define Aim_sw    2
#define Aim_start 3

#define Circle_ss1   0
#define Circle_ss2   1
#define Circle_pw    2
#define Circle_jg    3

#define NONE   100

#define Angle_Swing_BLUE  0
#define Angle_Swing_RED   1
#define Angle_Pole        2
#define Angle_Circle_Jg   3

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "MyTypeDef.h"
#include "includes.h"
/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern s16 Aim_X_Handover[4];
extern s16 Aim_Y_Handover[4];
extern s16 Aim_Z_Handover[4];
extern s16 Aim_A_Handover[4];
extern s16 Aim_X_Pole;
extern s16 Aim_Y_Pole;
extern fp64 Aim_A_Pole;
extern fp64 Aim_D_Swing[3];
extern fp64 Aim_A_Swing[2];
extern fp64 Aim_D_Circle[4];
extern fp64 Aim_A_Circle[4];
extern fp64 Aim_A_G_Circle[4];
extern double Aim_Angle;
extern double Aim_Dist;
extern fp64 ratio1[3],ratio2[3];

/* Exported function prototype -----------------------------------------------*/
void SetKeep(struct Point end_point,double aim_radian,double speed_max);
void Keep(void);
void SetHandover(s16 aim_x,s16 aim_y,s16 aim_z,s16 aim_a,int place);
void Keep_Handover(int place);
void SetPole(s16 aim_x,s16 aim_y,fp64 aim_a);
void Keep_Pole(void);
void SetSwing(fp64 aim_d1,fp64 aim_d2,fp64 aim_a,u8 ground);
void Keep_Swing(u8 ground);
void SetCamCircle(fp64 aim_a,fp64 aim_d,fp64 aim_a_g,u8 place);
void SetGoCamCircle(fp64 aim_a,fp64 aim_d);
void Keep_Circle(u8 place);
void Keep_Angle(u8 place);
#endif 

/**********************************END OF FILE*********************************/
