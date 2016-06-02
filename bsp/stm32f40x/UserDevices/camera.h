#ifndef __CAMERA_H
#define __CAMERA_H

#define CAMERA_LENGTH  8

#define LINE          1
#define CIRCLE        2

/********************************************************************************
camera ID:
1   forward camera for circle and whitetape
2   side camera for whitetape in the blue ground
3   side camera for whitetape in the red ground
4   poleput camera
5   handover camera
*********************************************************************************/

/**-------------------------------------------------------------+
 | @attention be cut to "includes.h"  by fxh_bool 4-18 01:23    |
 | #define  BLUE_GROUND        1                                |
 | #define  RED_GROUND         2                                |
 +--------------------------------------------------------------*/

/*for the Cam_Angle and Cam_Dist to differ*/
#define  Cam_Circle_Line         0
#define  Cam_Line_BLUE      		 1
#define  Cam_Line_RED      		   2

/*for the forward cam to choose function*/
#define Detect_Circle     0x00
#define Detect_WhiteTape  0xff

/*for the function CamRecode*/
#define Recode_Circle    0   
#define Recode_Handover  1
#define Recode_Pole      2
#define Recode_WhiteTape 3

/*for most function in camera.c*/
#define TEST         0
#define ARMACTION    1

typedef struct 
{
   struct Point center;
   fp64 r;
   u8 num;
}Facility;

extern fp64 Cam_Angle[3];
extern fp64 Cam_Dist[3];
extern s16 Cam_X,Cam_Y,Cam_Z,Cam_A;
extern s16 Cam_Pole_X,Cam_Pole_Y;
extern u8 CamFlag;
extern fp64 cam_angle;

extern Facility seesaw;
extern Facility junglegym;
extern Facility polewalk;
extern Facility swing;
extern struct Point Gps_assume[5];

extern void Gps_Camera(Facility circle,u8 ground,u8 mode);
extern void Go_CamCircle(void);
extern void ChooseFunction(u8 function);
extern void Handover(u8 function,u8 place);
extern void ShowInfo(void);
extern void PolePut(u8 function);
extern void CamCircle_Static(u8 function,u8 place);
extern void CamCircle_Dynamic(u8 function);
extern void CamWhiteTape(u8 function);
extern void ShowData(void);
extern void CamRecode(u8 function,u8 place,u8 FUNCTION);
extern void ChooseGround_Pole(u8 ground);
extern void PolePutTest(void);
extern void CamLED_Toggle(void);
extern void CamLED_Off(void);
extern void CamLED_On(void);
extern void _show_res(u8 res);


/*-----------------------------------------*
 * camera mode and camera data length      *
 *-----------------------------------------*/
#include "stm32f4xx.h"
#include "includes.h"
#pragma pack (1)
typedef union 
{
    u8 d[2];
    s16 v;
}u8tos16_t;

typedef struct
{
    u8tos16_t	x_or_angle;
    u8tos16_t	y_or_dis;
    u8tos16_t	z;
	u8tos16_t	turn;  
    int time_flag;
}camera_t;

extern camera_t camera_data1;
extern camera_t camera_data2;
extern camera_t camera_data3;
extern camera_t camera_data4;
extern camera_t camera_data5;
extern camera_t camera_data6;

#pragma pack ()


#endif 
