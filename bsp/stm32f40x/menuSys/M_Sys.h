/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENUSYS_H
#define __MENUSYS_H

/* Includes ------------------------------------------------------------------*/
/* variable ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

void sys_Gyro(void);
void sys_Encoder(void);
void sys_GPS(void);
void sys_Mecanum(void);
void sys_Swedish(void);
void sys_SA_ST(void);
void sys_CAN(void);
void sys_Handle(void);
void sys_Fan(void);
extern void RotatePID(void);
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#endif 

/**********************************END OF FILE*********************************/
