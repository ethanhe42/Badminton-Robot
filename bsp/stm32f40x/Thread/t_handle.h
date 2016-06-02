
#ifndef __T_HANDLE_H_
#define __T_HANDLE_H_

#include "stm32f4xx.h"

#define Button1_Up    ((HandleData_X.rest[0]&0x03)==0x01)
#define Button1_Down  ((HandleData_X.rest[0]&0x03)==0x02)
#define Button1_Off   ((HandleData_X.rest[0]&0x03)==0x0)
#define Button2_Up    ((HandleData_X.rest[0]&0x0c)==0x04)
#define Button2_Down  ((HandleData_X.rest[0]&0x0c)==0x08)
#define Button2_Off   ((HandleData_X.rest[0]&0x0c)==0x0)
#define Button3_Up    ((HandleData_X.rest[0]&0x30)==0x10)
#define Button3_Down  ((HandleData_X.rest[0]&0x30)==0x20)
#define Button3_Off   ((HandleData_X.rest[0]&0x30)==0x0)
#define Button4_Up    ((HandleData_X.rest[0]&0xc0)==0x40)
#define Button4_Down  ((HandleData_X.rest[0]&0xc0)==0x80)
#define Button4_Off   ((HandleData_X.rest[0]&0xc0)==0x0)

#define Button5_Up    ((HandleData_X.rest[1]&0x03)==0x01)
#define Button5_Down  ((HandleData_X.rest[1]&0x03)==0x02)
#define Button5_Off   ((HandleData_X.rest[1]&0x03)==0x0)
#define Button6_Up    ((HandleData_X.rest[1]&0x0c)==0x04)
#define Button6_Down  ((HandleData_X.rest[1]&0x0c)==0x08)
#define Button6_Off   ((HandleData_X.rest[1]&0x0c)==0x0)

#define Button10_On    ((HandleData_X.rest[1]&0x10)==0x10)
#define Button10_Off   ((HandleData_X.rest[1]&0x10)==0x0)
#define Button9_On    ((HandleData_X.rest[1]&0x20)==0x20)
#define Button9_Off   ((HandleData_X.rest[1]&0x20)==0x0)
#define Button8_On    ((HandleData_X.rest[1]&0x40)==0x40)
#define Button8_Off   ((HandleData_X.rest[1]&0x40)==0x0)
#define Button7_On   ((HandleData_X.rest[1]&0x80)==0x80)
#define Button7_Off  ((HandleData_X.rest[1]&0x80)==0x0)


#define Button8_State (HandleData_X.rest[1]&0x40)

int rt_thread_handle_init(void);
void Find_Handle_Key(void);
extern struct rt_thread thread_handle;
extern double Handle_Speed_X;
void Wait_Button8_Change(void);
#endif
