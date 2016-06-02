#ifndef __T_CATCHING_H
#define __T_CATCHING_H

#include "stm32f4xx.h"
#include "can_database.h"

int rt_thread_catch_init(void);
void RecKin_Print(void);
void Data_Processing(void);
void Clear_Data(void);
void Rec_Kin(void);
void Data_Pro_Text(void);
void Text_Catch(void);
void LookIt(void);
extern struct rt_thread thread_catch;
extern int AngleSwing;
extern int AngleHit;
extern struct pos_speed pos_speed;
#endif
