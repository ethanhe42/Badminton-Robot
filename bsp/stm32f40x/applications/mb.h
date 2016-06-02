/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MB_H
#define __MB_H

/* Includes ------------------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern struct rt_mailbox Mb_Key;
extern struct rt_mailbox Mb_Emergency;
extern struct rt_mailbox Mb_Auto;
extern struct rt_mailbox Mb_Arm;
extern struct rt_mailbox Mb_SensorEnd;
extern struct rt_mailbox Mb_SensorStart;
/* Exported function prototype -----------------------------------------------*/
/* Exported function prototype -----------------------------------------------*/
int Mb_Key_Init(void);
int Mb_Emergency_Init(void);
int Mb_Auto_Init(void);
int Mb_Arm_Init(void);
int Mb_SensorStart_Init(void);
int Mb_SensorEnd_Init(void);
#endif 

/**********************************END OF FILE*********************************/

