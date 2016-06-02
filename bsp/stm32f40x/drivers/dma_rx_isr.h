

#ifndef __DMA_RX_ISR_H_
#define __DMA_RX_ISR_H_

#include "stm32f4xx.h"

#pragma pack (1)
typedef struct
{
    u8  fh[2];  /* frame head */
    u32 v;
}LB_t;

typedef struct
{
	u8  fh;
	u16 ud;		/* up down */
	u16 lr;		/* left right */
	u16 turn;
	
	u8 rest[9];
}Handle_Data;

typedef struct
{
    u8 fh;
    u8 data[3];
    u8 sum;
}laser_t;

typedef struct
{
    u8 fh;
    u8 io;
    float p;
    s16 v;
    u8 sum;
}motor_t;

#pragma pack()

extern LB_t LB_recv[];
extern Handle_Data HandleData_X,HandleData_Y;
extern laser_t Laser_recv[];
extern motor_t mt2;
#endif
