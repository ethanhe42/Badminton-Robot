
#include "includes.h"
#include "math.h"


ALIGN(RT_ALIGN_SIZE)
static char thread_handle_stack[1024];
struct rt_thread thread_handle;

double Handle_Speed_X;
u8 handle_timeout_flag;

extern u8 Hall_Count;
extern u8 Retry;
extern 	u8 time_flag_y;
int Only_XY = 1;
u8 time_flag = 1;
int Handle_Off_Count=0;

extern u8 HandoverFlag[];
extern u8 PoleFlag;
extern u8 SwingFlag;
u8 fan_flag=0;//对应涵道的运行状态

/*
 * 函数名: Button8_Change
 * 描  述: 等待8号键变化
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 * 说  明: 
 */
void Wait_Button8_Change(void)
{
	u8 temp;
	temp = HandleData_X.rest[1]&0x40;
	
	while((HandleData_X.rest[1]&0x40) == temp)
		Delay_ms(5);
}

/*
 * 函数名: rt_thread_entry_handle
 * 描  述: 手柄线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 * 说  明: 
 */
static void rt_thread_entry_handle(void* parameter)
{
	double Handle_Speed_Y2;
	double Handle_Speed_Rotation;
//  struct Point temp_point;
//	struct Point now_point;
	u32 temp;
//	u8 delay_flag = 1;
//	u8 hall_flag = 0;
	u8 handleoff_flag = 1;
//	u8 flag[6] = {0}; //对应电机的运行状态
	int16_t Speed_M = 500;
//	float pos_openfan = 4;
//	extern float Motor2_Aim_Pos[];
//	extern u8 Motor2_Time_Flag;
	Handle_Speed_X = 0;
	TIM2->CNT = 0; 
	
    while(1)
    {
		temp = *handle_count;
		while(*handle_count == temp)
		{
			time_flag++;
			Delay_ms(5);
			if(time_flag > 60)//超时
			{
				time_flag = 0;
				handle_timeout_flag = 1;
				if(handleoff_flag)
				{
					Handle_Off_Count++;
					handleoff_flag = 0;
				}
				break;
			}
		}

		if(time_flag)//手柄工作正常
		{
			time_flag = 1;
			handle_timeout_flag = 0;
			handleoff_flag = 1;
			Handle_Speed_X = HandleData_X.lr-12288;
			Handle_Speed_Y2 = HandleData_X.ud-12288;
			Handle_Speed_Rotation = (HandleData_X.turn-12288)/10.0;

			if(Button10_On && Button7_Off)
			{
				Handle_Speed_X /= 10.0;
				Speed_M = 150;
			}
			else
				Speed_M = 500;
/*单手柄和Y手柄失灵紧急状况 实现单手柄控制*/
			if(Button9_On || time_flag_y == 0)
			{
				Only_XY = 0;
				SetSpeed(Speed_X+Handle_Speed_X,Speed_Y+Handle_Speed_Y2,Speed_Rotation+Handle_Speed_Rotation);
			}
			if(Button9_Off && time_flag_y != 0)
			{
				Only_XY = 1;
			}
/*y手柄失灵，X手柄有效的紧急状况（疯转等）制动 */
			if (Button6_Up)
			{
				SPEED_STOP;
				SetSpeed(0,0,0);
				Moto_Stop(W_MOTOR1_OLD_ID);
				Moto_Stop(W_MOTOR2_OLD_ID);
			}			
//			if(Button1_Up && flag[1]==0)
// 			{     
// 		  W_MOTOR_OLD_FUNC(0xa0,1,100,CMD_SET_PSG);
// 			}
// 			if(Button1_Down && flag[1]==0)
// 			{
// 				W_MOTOR_OLD_FUNC(0xa0,-1,100,CMD_SET_PSG);
//			}
    }
/*x,y手柄失灵的紧急状况（疯转等）制动 */
		else if (time_flag == 0 && time_flag_y == 0)
		{
			SPEED_STOP;
			SetSpeed(0,0,0);
			Moto_Stop(W_MOTOR1_OLD_ID);
			Moto_Stop(W_MOTOR2_OLD_ID);
    }
   Delay_ms(10);
	}
}

void Find_Handle_Key(void)
{
	rt_uint32_t key_value;

	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======key========");
		LCD_SetXY(0,1);
		LCD_Printf("X Handle Key:");
		LCD_SetXY(0,3);
		if(Button1_Up)
			LCD_Printf("%d",1);
		if(Button2_Up)
			LCD_Printf("%d",2);
		if(Button3_Up)
			LCD_Printf("%d",3);
		if(Button4_Up)
			LCD_Printf("%d",4);
		if(Button5_Up)
			LCD_Printf("%d",5);
		if(Button6_Up)
			LCD_Printf("%d",6);
		if(Button7_On)
			LCD_Printf("%d",7);
		if(Button8_On)
			LCD_Printf("%d",8);
		if(Button9_On)
			LCD_Printf("%d",9);
		if(Button10_On)
			LCD_Printf("%d",10);
		LCD_SetXY(0,4);
		LCD_Printf("Y Handle Key:");
		LCD_SetXY(0,5);
		if(Y_Button1_Up)
			LCD_Printf("%d",1);
		if(Y_Button2_Up)
			LCD_Printf("%d",2);
		if(Y_Button3_Up)
			LCD_Printf("%d",3);
		if(Y_Button4_Up)
			LCD_Printf("%d",4);
		if(Y_Button5_Up)
			LCD_Printf("%d",5);
		if(Y_Button6_Up)
			LCD_Printf("%d",6);
		if(Y_Button7_On)
			LCD_Printf("%d",7);
		if(Y_Button8_On)
			LCD_Printf("%d",8);
		if(Y_Button9_On)
			LCD_Printf("%d",9);
		if(Y_Button10_On)
			LCD_Printf("%d",10);

		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}
/*
 * 函数名: rt_thread_handle_init
 * 描  述: 手柄线程的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_handle_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_handle, 
                            "thread_handle",
                            rt_thread_entry_handle,
                            RT_NULL,
                            &thread_handle_stack[0], sizeof(thread_handle_stack),
							7, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_handle);
    else
        return RT_ERROR;

    return RT_EOK;
}
/********************************    END OF FILE    ***************************/
