/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_H
#define __MOTOR_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include "can_database.h"

#define     G1M        0   /* group_1 manual robot */
#define     G1A        1
#define     G2M        2
#define     G2A        3

/* command */
#define CMD_STOP        0
#define CMD_DEFAULT     1   //set all value to default value
#define CMD_SET_PN      2   //set now position to xxx
#define CMD_SET_PG      3   //set goal positon to xxx
#define CMD_SET_SG      4   //set goal speed to xxx
#define CMD_SET_PSG     5   //set goal speed and goal position
#define CMD_INIT_CAN    7   //Init can

#define     CURRENT_ROBOT   G1M 


void Moto_Stop(ID_NUMDEF ID_NUM);
void Moto_Set_NPos(ID_NUMDEF ID_NUM , float rot);	//set now position to xxx
void Moto_Clear_NPos(ID_NUMDEF ID_NUM);	//set all value to default value
void Moto_Set_GPos(ID_NUMDEF ID_NUM, float rot );	//goal possition
void Moto_Set_GSpeed(ID_NUMDEF ID_NUM, int16_t speed );   //goal speed

void Motor_FHB_Init(void);

void Motor_View(void);

void Motor_Init(void);

void MotorLED_On(void);
void MotorLED_Off(void);
void MotorLED_Toggle(void);

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/

extern u8		IO_State[];//电机控制盒上的ST的状态，比如:电机1对应 IO_State[0]
extern float	Motor_Pos[];//电机的绝对位置，比如:电机1对应 Motor_Pos[0]
extern int16_t Motor_RealAim[];
#endif 

/**********************************END OF FILE*********************************/

