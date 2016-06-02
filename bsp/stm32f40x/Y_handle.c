
#include "includes.h"
#include "math.h"

ALIGN(RT_ALIGN_SIZE)
static char thread_handle_stack_Y[1024];
struct rt_thread thread_handle_Y;
extern int fantest;
u8 handle_timeout_flag_y;
double Handle_Speed_Y;
extern int single;
extern int Only_XY;
double fanmax=5;
u8 time_flag_y = 1;
extern 	u8 time_flag;
int Handle_Off_Count_y=0;
/*
 * 函数名: Button8_Change
 * 描  述: 等待8号键变化
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 * 说  明: 
 */
void Wait_Y_Button8_Change(void)
{
	u8 temp;
	temp = HandleData_Y.rest[1]&0x40;
	
	while((HandleData_Y.rest[1]&0x40) == temp)
		Delay_ms(5);
}
	static double lastspeed_x=0,lastspeed_y=0;

/*
 * 函数名: rt_thread_entry_handle
 * 描  述: 手柄线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 * 说  明: 
 */
static void rt_thread_entry_handle_y(void* parameter)
{
	/*赋手柄速度的变量*/
//	double Handle_Speed_X;

	double Handle_Speed_X2;
	double Handle_Speed_Y2;
	double Handle_Speed_Rotation;
	u32 temp_y;
	u8 handleoff_flag_y = 1;
	int16_t Speed_M = 500;
	double nowspeed_x,nowspeed_y;
/*调节PID的变量*/
	double error_angle_this = 0;
	double Vout_aa = 0;
	struct PID pid_Keep;
	pid_Keep.i = 1000000;
	pid_Keep.p = -6;
	pid_Keep.d = 5;
//	Handle_Speed_X = 0;
	Handle_Speed_Y = 0;
	TIM2->CNT = 0;

    while(1)
    {
		temp_y = *handle_count_y;
		while(*handle_count_y == temp_y)
		{
			time_flag_y++;
			Delay_ms(5);
			if(time_flag_y > 60)//超时
			{
				time_flag_y = 0;
				handle_timeout_flag_y = 1;
				if(handleoff_flag_y)
				{
					Handle_Off_Count_y++;
					handleoff_flag_y = 0;
				}
				break;
			}
		}
		
		if(time_flag_y)//手柄工作正常
		{
			time_flag_y = 1;
			handle_timeout_flag_y = 0;
			handleoff_flag_y = 1;
			Handle_Speed_Y = HandleData_Y.lr-12288;
			Handle_Speed_X2 = HandleData_Y.lr-12288;
			Handle_Speed_Y2 = HandleData_Y.ud-12288;
			Handle_Speed_Rotation = (HandleData_Y.turn-12288)/10.0;


			if(Y_Button10_On && Y_Button7_Off)
			{
 				Handle_Speed_Y /= 10.0;
				Speed_M = 150;
			}
			else
				Speed_M = 500;
			if(Y_Button9_On || time_flag == 0)
			{
				Only_XY = 0;
				SetSpeed(Speed_X+Handle_Speed_X2,Speed_Y+Handle_Speed_Y2,Speed_Rotation+Handle_Speed_Rotation);
			}
			if(Y_Button9_Off && time_flag != 0)
			{
				Only_XY = 1;
			}
			if (Y_Button6_Up)
			{
				SPEED_STOP;
				SetSpeed(0,0,0);
				Moto_Stop(W_MOTOR1_OLD_ID);
				Moto_Stop(W_MOTOR2_OLD_ID);
			}
				if(single) 
				{
					Handle_Speed_X=	HandleData_Y.lr-12288;
					if(Y_Button10_On && Y_Button7_Off) Handle_Speed_X /= 10.0;
					nowspeed_x=Speed_X + Handle_Speed_X;
	
					nowspeed_y=Speed_Y + Handle_Speed_Y;
					if(Abs(lastspeed_x-nowspeed_x)>fanmax){lastspeed_y=nowspeed_y;
						lastspeed_x=lastspeed_x+(Abs(lastspeed_x-nowspeed_x)/(lastspeed_x-nowspeed_x))*fanmax;
					}
					else if(Abs(lastspeed_y-nowspeed_y)>fanmax){lastspeed_x=nowspeed_x;
						lastspeed_y=lastspeed_y+(Abs(lastspeed_y-nowspeed_y)/(lastspeed_y-nowspeed_y))*fanmax;
					}
					else
					{
						lastspeed_x=nowspeed_x;
						lastspeed_y=nowspeed_y;
					}
					if(fantest) FanAccelerate((nowspeed_x-lastspeed_x)/0.4095,(nowspeed_y-lastspeed_y)/0.4095);
					 SetSpeed(nowspeed_x,nowspeed_y,-2*Speed_Rotation);
	
				}
				else if(Only_XY == 1) 
				{
					error_angle_this = (0 - GPS.radian);
					if(error_angle_this>pi)
					error_angle_this-=2*pi;
					if(error_angle_this<-pi)
					error_angle_this+=2*pi;
					if(Abs(error_angle_this)<0.002)
					Speed_Rotation = 0;
					else
					{
						Vout_aa = 100 * AnglePID(error_angle_this,pid_Keep);
						Speed_Rotation = Vout_aa;
					}
					SetSpeed(Speed_X + Handle_Speed_X*cos(GPS.radian) + Handle_Speed_Y*sin(GPS.radian),Speed_Y + Handle_Speed_Y*cos(GPS.radian)-Handle_Speed_X*sin(GPS.radian),-Speed_Rotation);
				}
		}
		else if(time_flag == 0 && time_flag_y ==0)
		{
			SPEED_STOP;
			//Acc_Limit_Enable = 0;
			SetSpeed(0,0,0);
			//Acc_Limit_Enable = 1;
			Moto_Stop(W_MOTOR1_OLD_ID);
			Moto_Stop(W_MOTOR2_OLD_ID);
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
int rt_thread_handle_Y_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_handle_Y, 
                            "thread_handle_Y",
                            rt_thread_entry_handle_y,
                            RT_NULL,
                            &thread_handle_stack_Y[0], sizeof(thread_handle_stack_Y),
							8, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_handle_Y);
    else
        return RT_ERROR;

    return RT_EOK;
}
/********************************    END OF FILE    ***************************/
