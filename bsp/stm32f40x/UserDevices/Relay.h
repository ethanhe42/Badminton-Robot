/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RELAY_H
#define __RELAY_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

#define     RELAY_NUM(port)    (uint32_t)(0x1<<(port-1)) //该宏定义用来选择继电器口对应的端位，与Relay()这个函数一起用

void Relay_Multi(uint64_t set_num,uint64_t clear_num);
void Relay_Set(uint8_t port);
void Relay_Clear(uint8_t port);
void Relay_View(void);
u8 relay_attain(u8 port);
void HandOpen(void);
void HandClose(void);
void HandTurnRight(void);
void HandTurnLeft(void);
void HandPush(void);
void HandPushOff(void);
void HandRaise(void);
void HandFall(void);
void BufferOn(void);
void BufferOff(void);
void GunOn(void);
void GunOff(void);
void FireOn(void);
void FireOff(void);
void FireClear(void);

void Hall_Send( u8 data);

#endif 

/**********************************END OF FILE*********************************/

