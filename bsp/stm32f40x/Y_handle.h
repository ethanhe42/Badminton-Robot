
#ifndef __Y_HANDLE_H_
#define __Y_HANDLE_H_

#include "stm32f4xx.h"

#define Y_Button1_Up    ((HandleData_Y.rest[0]&0x03)==0x01)
#define Y_Button1_Down  ((HandleData_Y.rest[0]&0x03)==0x02)
#define Y_Button1_Off   ((HandleData_Y.rest[0]&0x03)==0x0)
#define Y_Button2_Up    ((HandleData_Y.rest[0]&0x0c)==0x04)
#define Y_Button2_Down  ((HandleData_Y.rest[0]&0x0c)==0x08)
#define Y_Button2_Off   ((HandleData_Y.rest[0]&0x0c)==0x0)
#define Y_Button3_Up    ((HandleData_Y.rest[0]&0x30)==0x10)
#define Y_Button3_Down  ((HandleData_Y.rest[0]&0x30)==0x20)
#define Y_Button3_Off   ((HandleData_Y.rest[0]&0x30)==0x0)
#define Y_Button4_Up    ((HandleData_Y.rest[0]&0xc0)==0x40)
#define Y_Button4_Down  ((HandleData_Y.rest[0]&0xc0)==0x80)
#define Y_Button4_Off   ((HandleData_Y.rest[0]&0xc0)==0x0)

#define Y_Button5_Up    ((HandleData_Y.rest[1]&0x03)==0x01)
#define Y_Button5_Down  ((HandleData_Y.rest[1]&0x03)==0x02)
#define Y_Button5_Off   ((HandleData_Y.rest[1]&0x03)==0x0)
#define Y_Button6_Up    ((HandleData_Y.rest[1]&0x0c)==0x04)
#define Y_Button6_Down  ((HandleData_Y.rest[1]&0x0c)==0x08)
#define Y_Button6_Off   ((HandleData_Y.rest[1]&0x0c)==0x0)

#define Y_Button10_On    ((HandleData_Y.rest[1]&0x10)==0x10)
#define Y_Button10_Off   ((HandleData_Y.rest[1]&0x10)==0x0)
#define Y_Button9_On    ((HandleData_Y.rest[1]&0x20)==0x20)
#define Y_Button9_Off   ((HandleData_Y.rest[1]&0x20)==0x0)
#define Y_Button8_On    ((HandleData_Y.rest[1]&0x40)==0x40)
#define Y_Button8_Off   ((HandleData_Y.rest[1]&0x40)==0x0)
#define Y_Button7_On   ((HandleData_Y.rest[1]&0x80)==0x80)
#define Y_Button7_Off  ((HandleData_Y.rest[1]&0x80)==0x0)


#define Y_Button8_State (HandleData_Y.rest[1]&0x40)

int rt_thread_handle_Y_init(void);

extern struct rt_thread thread_handle_Y;
extern double Handle_Speed_Y;
void Wait_Y_Button8_Change(void);

#endif
