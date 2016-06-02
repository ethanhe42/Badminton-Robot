
#include "includes.h"
#include "math.h"

ALIGN(RT_ALIGN_SIZE)
static char thread_handle_stack[1024];
struct rt_thread thread_handle;

u8 handle_timeout_flag;
u8 handle_timeout_flag2;

extern u8 Hall_Count;
extern u8 Retry;

int Handle_Off_Count=0;
int Handle_Off_Count2=0;

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
	double Handle_Speed_X;
	double Handle_Speed_Y;
	double Handle_Speed_Rotation;
	
	struct Point temp_point;
	struct Point now_point;
	u32 temp,temp2;
	u8 delay_flag = 1;
	u8 hall_flag = 0;
	u8 time_flag = 1,time_flag2 = 1;
	u8 handleoff_flag = 1,handleoff_flag2 = 1;
	u8 flag[6] = {0}; //对应电机的运行状态
	int16_t Speed_M = 500;
	float pos_openfan = 4;
	extern float Motor2_Aim_Pos[];
	extern u8 Motor2_Time_Flag;
	
	u8 x_con_flag = 0;	//X方向闭环
	u8 XY_Only = 0;
	Handle_Speed_X = 0;
	Handle_Speed_Y = 0;
  HandleData_X.lr= 12288;
  HandleData_Y.lr= 12288;
	TIM2->CNT = 0;
	
    while(1)
    {
		temp = *handle_count;
		temp2 = *handle_count2;
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
				while(*handle_count2 == temp2)
		{
			time_flag2++;
			Delay_ms(5);
			if(time_flag2 > 60)//超时
			{
				time_flag2 = 0;
				handle_timeout_flag2 = 1;
				if(handleoff_flag2)
				{
					Handle_Off_Count2++;
					handleoff_flag2 = 0;
				}
				break;
			}
		}
		
		if(time_flag||time_flag2)//手柄工作正常
		{
			time_flag = 1;
			time_flag2 = 1;
			handle_timeout_flag = 0;
			handle_timeout_flag2 = 0;
			handleoff_flag = 1;
			handleoff_flag2 = 1;
			Handle_Speed_X = HandleData_X.lr-12288;
			Handle_Speed_Y = HandleData_Y.lr-12288;
			Handle_Speed_Rotation = 0;
			//Handle_Speed_Y = HandleData_Y.ud-12288;
			//Handle_Speed_Rotation = (HandleData_X.turn-12288)/10.0;
			
			if(Button10_On && Button7_Off)
			{
				Handle_Speed_X /= 10.0;
				Handle_Speed_Y /= 10.0;
				Handle_Speed_Rotation /= 10.0;
				Speed_M = 150;
			}
			else
				Speed_M = 500;
			
			if(Button9_On)
			{
//				HandoverFlag[0]=1;
//				HandoverFlag[1]=1;
//				HandoverFlag[2]=0;
//				PoleFlag = 1;
//				SwingFlag = 1;
			}
			if(Button9_Off)
			{
//				HandoverFlag[0]=0;
//				HandoverFlag[1]=0;
//				HandoverFlag[2]=0;
//				PoleFlag = 0;
//				SwingFlag = 0;
			}
			
			if(XY_Only)
			{
			//	Handle_Speed_X=0;
				Handle_Speed_Rotation=0;
			}
			
			if(GROUND==RED_GROUND)
			{
				Handle_Speed_X = -Handle_Speed_X;
				Handle_Speed_Y = -Handle_Speed_Y;
			}
			
			if(x_con_flag)
			{
				temp_point.x = GPS.position.x + Handle_Speed_X*cos(GPS.radian);
				temp_point.y = GPS.position.y + Handle_Speed_X*sin(GPS.radian);
				
				SetSpeed(Speed_X,Speed_Y+Handle_Speed_Y,Speed_Rotation+Handle_Speed_Rotation);
				
				EasyLine(temp_point,GPS.radian,Abs(Handle_Speed_X));
			}
			else
 				SetSpeed(Speed_X+Handle_Speed_X,Speed_Y+Handle_Speed_Y,Speed_Rotation+Handle_Speed_Rotation);
          //SetSpeed(Speed_X,Speed_Y,Speed_Rotation+Handle_Speed_Rotation);   //手柄前后出现问题，先无视之。。。
			
			/*********************************上层动作控制***********************************/
			
			//------------单键控制电机------------//
			if(Button1_Up && Button7_Off && flag[1]==0)
			{
				flag[1]=1;
			    Moto_Set_GSpeed(W_MOTOR1_OLD_ID, 4*Speed_M);
			}
			if(Button1_Down && Button7_Off && flag[1]==0)
			{
				flag[1]=1;
			    Moto_Set_GSpeed(W_MOTOR1_OLD_ID, -4*Speed_M);
			}
			if(Button1_Off && (flag[1]==1))
			{
				flag[1]=0;
				Moto_Stop(W_MOTOR1_OLD_ID);
			}

			
			if(Button2_Up && Button7_Off && flag[2]==0)
			{
				flag[2]=1;
			    Moto_Set_GSpeed(W_MOTOR2_OLD_ID, -3*Speed_M);
			}
			if(Button2_Down && Button7_Off && flag[2]==0)
			{
				flag[2]=1;
			    Moto_Set_GSpeed(W_MOTOR2_OLD_ID, 3*Speed_M);
			}
			if(Button2_Off && (flag[2]==1))
			{
				Moto_Stop(W_MOTOR2_OLD_ID);
				flag[2]=0;
			}
			
			
			if(Button3_Up && Button7_Off && flag[3]==0)
			{
				flag[3]=1;
			    Moto_Set_GSpeed(W_MOTOR3_OLD_ID, -30*Speed_M);
			}
			if(Button3_Down && Button7_Off && flag[3]==0)
			{
				flag[3]=1;
			    Moto_Set_GSpeed(W_MOTOR3_OLD_ID, 30*Speed_M);
			}
			if(Button3_Off && (flag[3]==1))
			{
				flag[3]=0;
				Moto_Stop(W_MOTOR3_OLD_ID);
			}
			
			//------------涵道------------//
			if(Button4_Up && Button7_Off && Button10_Off)
			{
			    Fan_Duty(L_CH,60.0);
				Fan_Duty(R_CH,60.0);
			}
			if(Button4_Down && Button7_Off && Button10_Off)
			{
			    Fan_Stop();
			}
			
			//------------霍尔------------//
			if(Button5_Up && Button7_Off && Button10_Off && hall_flag == 0)
			{
				hall_flag = 1;
				if(Hall_Count > 6) Hall_Count=1;
			    Hall_Send(Hall_Count);
			}
			if(Button5_Down && Button7_Off && Button10_Off && hall_flag == 0)
			{
				hall_flag = 1;
				if(Hall_Count > 1) Hall_Count -= 2;
				Hall_Send(Hall_Count);
			}
			if(Button5_Off && hall_flag == 1)
			{
				hall_flag = 0;
			}
			
		  	//------------双键组合用来控制电磁阀，7键为第二功能键------------//
			
			if(Button6_Up && Button7_Off && Button10_Off){
				HandPush();
			}
			if(Button6_Down && Button7_Off && Button10_Off){
				HandPushOff();
			}
			
			//缓冲装置
			if(Button1_Up && Button7_On && Button10_Off){
				BufferOn();
			}
			if(Button1_Down && Button7_On && Button10_Off){
				BufferOff();
			}
			//旋转调姿
		  	if(Button2_Up && Button7_On && Button10_Off){
				HandTurnRight();
			}
			if(Button2_Down && Button7_On && Button10_Off){
				HandTurnLeft();
			}
			//俯仰调姿
			if(Button3_Up && Button7_On && Button10_Off){
				HandRaise();
			}
			if(Button3_Down && Button7_On && Button10_Off){
				HandFall();
			}
			//放置大炮
			if(Button4_Up && Button7_On && Button10_Off){
			    GunOn();
			}
			
			if(Button4_Down && Button7_On && Button10_Off){
			    GunOff();
			}
			//开炮
     		if(Button5_Up && Button7_On && Button10_Off){
				FireOn();
			}
			
			if(Button5_Down && Button7_On && Button10_Off){
				FireOff();
			}
			
			
			//交接爪
			if(Button6_Up && Button7_On && Button10_Off){
			    HandOpen();
			}
			
			if(Button6_Down && Button7_On && Button10_Off){
			    HandClose();
			}
			
			//------------双键组合用来控制上下电机的位置，10键为第二功能键------------//
			
			if(Button4_Up && Button7_Off && Button10_On){
			    Moto_Set_GPos(W_MOTOR2_OLD_ID,Motor2_Aim_Pos[0]);
			}
			
			if(Button4_Down && Button7_Off && Button10_On){
			    Moto_Set_GPos(W_MOTOR2_OLD_ID,Motor2_Aim_Pos[2]);
			}
			if(Button5_Up && Button7_Off && Button10_On){
			    Moto_Set_GPos(W_MOTOR2_OLD_ID,Motor2_Aim_Pos[4]);
			}
			
			if(Button5_Down && Button7_Off && Button10_On){
			    Moto_Set_GPos(W_MOTOR2_OLD_ID,Motor2_Aim_Pos[6]);
			}
			
			if(Button6_Up && Button7_Off && Button10_On){
			    HandUD(0,NO);
				Delay_ms(500);
				HandClose();//关爪子
				HandFB(0,NO,3500);
				FireOn();//开炮
				Delay_ms(100);

				now_point.x = GPS.position.x-700*cos(GPS.radian);
				now_point.y = GPS.position.y-700*sin(GPS.radian);
				SetLine(now_point,GPS.radian,100,1200,0);
				
				while(1)
				{
					GoLine();
					SetSpeed(Speed_X+Handle_Speed_X,Speed_Y+Handle_Speed_Y,Speed_Rotation+Handle_Speed_Rotation);
					if(delay_flag)
						delay_flag++;
					if(delay_flag>100)
					{
						delay_flag = 0;
						FireOff();
					}
					if(GetLength(GPS.position,now_point) < 10)
					{
						SPEED_STOP;
						break;
					}
					Delay_ms(5);
				}
				delay_flag = 1;
				FireClear();
				
				Delay_ms(50);
				BufferOn();//打开缓冲装置
				Delay_ms(500);
				BufferOff();//重新复位缓冲装置
			}
			
			if(Button6_Down && Button7_Off && Button10_On){
			    now_point.x = GPS.position.x+680*cos(GPS.radian);
				now_point.y = GPS.position.y+680*sin(GPS.radian);
				SetLine(now_point,GPS.radian,300,1200,0);
				HandFB(Motor1_Aim_Pos[4],NO,3500);//抓取自动机器人
				while(1)
				{
					GoLine();
					SetSpeed(Speed_X+Handle_Speed_X,Speed_Y+Handle_Speed_Y,Speed_Rotation+Handle_Speed_Rotation);
					if(GetLength(GPS.position,now_point) < 10)
					{
						SPEED_STOP;
						break;
					}
					Delay_ms(5);
				}
				SetSpeed(Speed_X,Speed_Y,Speed_Rotation);
				
				HandUD(0.5,NO);
				HandOpen();//松开爪子
				
				Delay_ms(300);
				FireOff();
				BufferOn();//打开缓冲装置
			}
			
			
			//三键组合用来重复任务
			if(Button1_Up && Button7_On && Button10_On){
				Retry = 1;
				RouteFinish=0;
				Hall_Send(1);//翘翘板初始化
				rt_mb_send(&Mb_Emergency, 7);
				rt_mb_send (&Mb_Arm, 1);
			}
			if(Button2_Up && Button7_On && Button10_On){
				Retry = 1;
				RouteFinish=0;
				Hall_Send(2);//梅花桩初始化
				rt_mb_send(&Mb_Emergency, 7);
				rt_mb_send (&Mb_Arm, 4);
			}
			if(Button3_Up && Button7_On && Button10_On){
				Retry = 1;
				RouteFinish=0;
				Hall_Send(4);//秋千初始化
				rt_mb_send(&Mb_Emergency, 7);
				rt_mb_send (&Mb_Arm, 6);
			}
			if(Button4_Up && Button7_On && Button10_On){
				Retry = 1;
				RouteFinish=0;
				Hall_Send(6);//楼梯初始化
				rt_mb_send(&Mb_Emergency, 7);
				rt_mb_send (&Mb_Arm, 7);
			}
			if(Button5_Up && Button7_On && Button10_On){
					rt_mb_send(&Mb_Emergency, 6);
			}
			if(Button6_Up && Button7_On && Button10_On){
				Retry = 1;
				RouteFinish=0;
				rt_mb_send(&Mb_Emergency, 7);
				//Motor_Init();
				FireOff();
				BufferOn();
				ChooseGround_Pole(GROUND);
				W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, 0.5 , 1000 , CMD_SET_PSG);
				Moto_Set_GPos(W_MOTOR2_OLD_ID,2.55);
				LCD_Clear();
				LCD_SetXY(0,1);
				LCD_Printf("Ready to go!");
				Wait_Button8_Change();
				rt_mb_send (&Mb_Auto, 1);
			}
		}
		
		else
		{
			SPEED_STOP;
			//Acc_Limit_Enable = 0;
			SetSpeed(0,0,0);
			//Acc_Limit_Enable = 1;
			Moto_Stop(W_MOTOR1_OLD_ID);
			Moto_Stop(W_MOTOR2_OLD_ID);
			//Moto_Stop(W_MOTOR3_OLD_ID);
			//Com_Printf(5,"v0\r");
		}
        
		if(Abs(Motor_Pos[0])>pos_openfan && CHILD_ON && !fan_flag)
		{
			fan_flag = 1;
			Fan_Duty(L_CH,60.0);
			Fan_Duty(R_CH,60.0);
		}
		if((Abs(Motor_Pos[0])<pos_openfan || CHILD_OFF) && fan_flag)
		{
			fan_flag = 0;
			Fan_Stop();
		}
		
		if(Motor2_Time_Flag>10)
		{
			//W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, 0 , 0 , CMD_INIT_CAN);
			Motor2_Time_Flag = 0;
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