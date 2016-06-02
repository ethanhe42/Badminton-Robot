/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"

double motor1_speed = 3500;
u8 Retry = 0;

static u8 Temp_button8_state;
static u8 Auto_flag = 1;
u8 debug = 0;

extern FIL cam_file;

extern struct Path_Point PointRoute[];
/****************************上层动作控制******************************/
//void Motor1_Zero(void)
//{
//	Moto_Set_GSpeed(W_MOTOR1_OLD_ID, -motor1_speed);
//	
//	while(M_IO_READ(W_MOTOR1_OLD_ID,1))
//		Delay_ms(5);
//	
//	Moto_Clear_NPos(W_MOTOR1_OLD_ID);
//}
//void Motor2_Zero(void)
//{
//	Moto_Set_GSpeed(W_MOTOR2_OLD_ID, -10000);
//	
//	while(M_IO_READ(W_MOTOR2_OLD_ID,3))
//		Delay_ms(5);
//	
//	Moto_Clear_NPos(W_MOTOR2_OLD_ID);
//}
void HandFB(float rotation,u8 state,int16_t speed)
{
	rt_uint32_t key_value;
	int i = 0;
//	u8 flag = 1;
	
	W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, rotation , speed , CMD_SET_PSG);
	
	if(state)
	{
		while(Abs(Motor_Pos[0]-rotation)>0.05f)
		{
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Motor1 pos error!");
			LCD_SetXY(0,2);
			LCD_Printf("NowPos:%.2f",Motor_Pos[0]);
			LCD_SetXY(0,3);
			LCD_Printf("AimPos:%.2f",rotation);
			i++;
			if(i==10)
			{
				i=0;
				MotorLED_Toggle();
			}
			
//			if(Abs(Motor_RealAim[0]/1000.0-rotation)>0.1f)
//				W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, rotation , motor1_speed , CMD_SET_PSG);
			
//			if(Abs(Motor_Pos[0]-rotation)<1.5 && flag)
//			{
//				flag = 0;
//				W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, rotation , motor1_speed , CMD_SET_PSG);
//			}
			
			if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
				break;
			Delay_ms(5);
		}
		MotorLED_On();
		Moto_Stop(W_MOTOR1_OLD_ID);
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Motor1 arrive!");
	}
}

void HandUD(float rotation,u8 state)
{
	rt_uint32_t key_value;
	
	//Moto_Set_GPos(W_MOTOR2_OLD_ID,rotation);
	W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, rotation , 10000 , CMD_SET_PSG);
	
	if(state)
	{
		while(Abs(Motor_Pos[1]-rotation)>0.05f)
		{
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Motor2 pos error!");
			LCD_SetXY(0,2);
			LCD_Printf("NowPos:%.2f",Motor_Pos[1]);
			LCD_SetXY(0,3);
			LCD_Printf("AimPos:%.2f",rotation);
			
			if(Abs(Motor_RealAim[1]/1000.0-rotation)>0.05f)
				W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, rotation , 10000 , CMD_SET_PSG);
			
			if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
				break;
			
			Delay_ms(5);
		}
		Moto_Stop(W_MOTOR2_OLD_ID);
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Motor2 arrive!");
	}
}

void Motor3(float rotation,u8 state,u8 port)
{
	rt_uint32_t key_value;
	
	W_MOTOR_OLD_FUNC(W_MOTOR3_OLD_ID, rotation , 6000 , CMD_SET_PSG);
	//Moto_Set_GPos(W_MOTOR3_OLD_ID,rotation);//位置模式（极速模式）
	
	if(state)
	{
		while(Abs(Motor_Pos[2]-rotation)>0.05f)
		{
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Motor3 pos error!");
			LCD_SetXY(0,2);
			LCD_Printf("NowPos:%.2f",Motor_Pos[2]);
			LCD_SetXY(0,3);
			LCD_Printf("AimPos:%.2f",rotation);
			
			if(Abs(Motor_RealAim[2]/1000.0-rotation)>0.05f)
				Moto_Set_GPos(W_MOTOR3_OLD_ID,rotation);//位置模式（极速模式）
			
			if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
				break;
            if(!M_IO_READ(W_MOTOR3_OLD_ID,port))
				break;
			
			if(Temp_button8_state != Button8_State)
			{
				Auto_flag = 0;
				Temp_button8_state = Button8_State;
			}
			
			Delay_ms(5);
		}
		
//		while(1)
//		{
//			LCD_Clear();
//			LCD_SetXY(0,1);
//			LCD_Printf("not reach!");
//			if(!M_IO_READ(W_MOTOR3_OLD_ID,port))
//				break;
//			Delay_ms(5);
//		}
			
		Moto_Stop(W_MOTOR3_OLD_ID);
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Motor3 arrive!");
	}
}



/****************************跑自动任务时的上层动作******************************/

extern u8 HandoverFlag[3];
extern u8 PoleFlag;
extern u8 SwingFlag;
extern u8 CircleFlag[4];
extern u8 Circle_dynFlag;
extern u8 LaserFlag[];
extern u8 WalkGround;

extern u8 armtest_flag;
extern u8 Cam_F_DISABLE;
extern fp64 SW_FORWARD_SPEED;
extern fp64 SW_FORWARD_DIS;

extern u8 TimeOnFlag;
extern int RunTime;
extern u8 CircleFinish;
int SegmentTime[7];
int RouteTime[7];

u8 FilterFlag[6];

double StartAngle;
struct GPS_State GPS_Record[7];
FIL fil_walkground;

#if GROUND==BLUE
//								ss放		ss取		pw放		pw取		sw放		jg放		
	float Motor1_Aim_Pos[] = { 4.470145,	3.000000,	3.171526,	4.210484,	4.283238,	1.070103};
	float Motor2_Aim_Pos[] = { 2.350500,	1.559000,	2.882000,	1.849500,	0.688500,	2.800500};
	float Motor3_Aim_Pos[] = {3,6.708116};
#elif GROUND==RED
//								ss放		ss取		pw放		pw取		sw放		jg放
	float Motor1_Aim_Pos[] = { 4.161447,	3.848493,	3.508231,	4.856794,	4.147217,	2.205801};
	float Motor2_Aim_Pos[] = { 2.368500,	1.454500,	2.868500,	1.792000,	0.673500,	2.618500};
	float Motor3_Aim_Pos[] = {3,6.70800};
	
#endif

void armaction1(void)
{
	int flag = 0;
	rt_uint32_t key_value;
	
	while(RouteFinish)
	{
		if(Point_NowNum>(Route[1]-500) && flag==0)
		{
			flag=1;
			HandFB(3.0/4.0*Motor1_Aim_Pos[0],NO,motor1_speed);
			HandUD(0.5,WAIT);
		}
		Delay_ms(5);
	}
	RouteTime[0] = RunTime;//记录第一段路径的时间
	
	if(WalkGround)
	{
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-SS-Put");
			LCD_SetXY(0,1);LCD_Printf("X:%f",GPS.position.x);
			LCD_SetXY(0,2);LCD_Printf("Y:%f",GPS.position.y);
			LCD_SetXY(0,3);LCD_Printf("A:%f",GPS.radian);
			LCD_SetXY(0,4);LCD_Printf("M1:%f",Motor_Pos[0]);
			LCD_SetXY(0,5);LCD_Printf("M2:%f",Motor_Pos[1]);
			Delay_ms(20);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		GPS_Record[0] = GPS;
		Motor1_Aim_Pos[0] = Motor_Pos[0];
		Motor2_Aim_Pos[0] = Motor_Pos[1];
	}
	
	if(Retry)
	{
		Retry = 0;
		HandFB(Motor1_Aim_Pos[0],WAIT,motor1_speed);
	}
	HandFB(Motor1_Aim_Pos[0],NO,motor1_speed);
	HandUD(Motor2_Aim_Pos[0],WAIT);//放自动车
	
	Wait_Button8_Change();
	HandOpen();//松开爪子
	
	while(CHILD_ON)//判断自动车是否脱离
		Delay_ms(5);
	
	HandUD(1,WAIT);
	HandFB(3.6,NO,motor1_speed);
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_WriteString("arm1 done!");
	
	
	LCD_SetXY(0,2);
	LCD_Printf("Time:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);
	
	if(debug)
		Wait_Button8_Change();
	
	if(armtest_flag)
	  rt_mb_send (&Mb_Auto, 2);
	
	SegmentTime[0] = RunTime;
}

void armaction2(void)
{
	int flag=0;
	rt_uint32_t key_value;
	
	if(Circle_dynFlag)
	{
		while(1)
		{
			if(Gyro.angle<20&&flag==0)
			{
				flag = 1;
				HandFB(3.6,NO,motor1_speed);
				HandUD(3.5,NO);
				Motor3(2.5,NO,1);
			}
			Delay_ms(5);
			if(!CircleFinish)
				break;
		}
	}
	else
	{
		while(RouteFinish)
		{
			if(Point_NowNum>(Route[1]+100)&&flag==0)
			{
				flag = 1;
				HandFB(3.6,NO,motor1_speed);
				HandUD(1.7,NO);
				Motor3(2.5,NO,1);
			}
			Delay_ms(5);
		}
	}
	RouteTime[1] = RunTime;//记录第二段路径的时间
	
	if(WalkGround)
	{
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-SS-Press-Up");
			LCD_SetXY(0,1);LCD_Printf("X:%f",GPS.position.x);
			LCD_SetXY(0,2);LCD_Printf("Y:%f",GPS.position.y);
			LCD_SetXY(0,3);LCD_Printf("A:%f",GPS.radian);
			LCD_SetXY(0,4);LCD_Printf("M3:%f",Motor_Pos[2]);
			Delay_ms(20);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		GPS_Record[1] = GPS;
		Motor3_Aim_Pos[0] = Motor_Pos[2];
		
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-SS-Press-Down");
			LCD_SetXY(0,4);LCD_Printf("M3:%f",Motor_Pos[2]);
			Delay_ms(20);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		Motor3_Aim_Pos[1] = Motor_Pos[2];
		//CamRecode(Recode_Circle,Circle_ss1,ARMACTION);
		//Wait_Button8_Change();
		//if(CircleFlag[Circle_ss1])
			//CamCircle_Static(ARMACTION,Circle_ss1);
	}
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Press button8 to continue");
	Wait_Button8_Change();
    
	Temp_button8_state = Button8_State;
	
	mt_line(W_MOTOR3_OLD_ID,Motor_Pos[2],Motor3_Aim_Pos[1],0,0.3,100,100,50);
	mt_line(W_MOTOR3_OLD_ID,Motor_Pos[2],Motor3_Aim_Pos[0],0.3,0.3,-50,-100,-50);
	mt_line(W_MOTOR3_OLD_ID,Motor_Pos[2],Motor3_Aim_Pos[1],0,0.3,100,100,50);
	mt_line(W_MOTOR3_OLD_ID,Motor_Pos[2],Motor3_Aim_Pos[0],0.3,0.3,-50,-100,-50);
	mt_line(W_MOTOR3_OLD_ID,Motor_Pos[2],Motor3_Aim_Pos[1],0,0.3,100,100,50);
	mt_line(W_MOTOR3_OLD_ID,Motor_Pos[2],Motor3_Aim_Pos[0],0.3,0.3,-50,-100,-50);
	
//	Motor3(Motor3_Aim_Pos[1],WAIT,1);
//	Motor3(Motor3_Aim_Pos[0],WAIT,3);
//	Motor3(Motor3_Aim_Pos[1],WAIT,1);
//	Motor3(Motor3_Aim_Pos[0],WAIT,3);
//	Motor3(Motor3_Aim_Pos[1],WAIT,1);
	Motor3(0,NO,3);
	
	while(1)
	{
		if(Abs(Motor_RealAim[2]/1000.0)>0.05f)
			Moto_Set_GPos(W_MOTOR3_OLD_ID,0);//位置模式（极速模式）
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
			break;
		if(!M_IO_READ(W_MOTOR3_OLD_ID,3))
			break;
		
		if(Temp_button8_state != Button8_State)
		{
			Auto_flag = 0;
			Temp_button8_state = Button8_State;
		}
		if(Abs(Motor_Pos[2])<Abs(Motor3_Aim_Pos[0]))
			break;
		
		Delay_ms(5);
	}
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_WriteString("arm2 done!");
    
 	LCD_SetXY(0,2);
	LCD_Printf("Time:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);
	
	if(Auto_flag)
		Wait_Button8_Change();
	Auto_flag = 1;
	
	if(armtest_flag)
		rt_mb_send (&Mb_Auto, 3);
	
	SegmentTime[1] = RunTime;
}

void armaction3(void)
{
	int flag=0;
	rt_uint32_t key_value;
	
	while(RouteFinish)
	{
		if(Point_NowNum>(Route[2]+Route[3])/2&&flag==0)
		{
			flag=1;
			HandFB(Motor1_Aim_Pos[1],NO,motor1_speed);
			HandUD(1,WAIT);
		}
		Delay_ms(5);
	}
	RouteTime[2] = RunTime;//记录第三段路径的时间
	
	if(WalkGround)
	{
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-SS-Get");
			LCD_SetXY(0,1);LCD_WriteString("Find a place camera can see");
			LCD_SetXY(0,2);LCD_Printf("X:%f",GPS.position.x);
			LCD_SetXY(0,3);LCD_Printf("Y:%f",GPS.position.y);
			LCD_SetXY(0,4);LCD_Printf("A:%f",GPS.radian);
			LCD_SetXY(0,5);LCD_Printf("M1:%f",Motor_Pos[0]);
            Delay_ms(20);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		GPS_Record[2] = GPS;
		Motor1_Aim_Pos[1] = Motor_Pos[0];
		LCD_Clear();
		
		CamRecode(Recode_Handover,Aim_ss,ARMACTION);
	}
	
	if(HandoverFlag[Aim_ss])//是否要在跷跷板交接的标志位
	{
		HandUD(Motor2_Aim_Pos[1],WAIT);
		Handover(ARMACTION,Aim_ss);
		
		//Wait_Button8_Change();
	}
	else//无摄像头交接，手动抓取和喂糖，然后自动归位
	{
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Ready to got the child?");
		Wait_Button8_Change();
		HandUD(Motor2_Aim_Pos[1]+1.5f,NO);
	}
	
	while(CHILD_OFF)//判断是否抓到自动车
		Delay_ms(5);
	Moto_Stop(W_MOTOR2_OLD_ID);
	HandClose();
 	Delay_ms(50);
// 	Hall_Send(2);//让自动车松开爪子 & 梅花桩初始化
// 	Delay_ms(50);
// 	Hall_Send(2);//（不知道为什么有时候不灵，所以发了两次命令）
// 	Delay_ms(50);
	
	HandUD(1.5,NO);
	Delay_ms(200);
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_WriteString("arm3 done!");
	
	LCD_SetXY(0,2);
	LCD_Printf("Time:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);
	
	if(debug)
		Wait_Button8_Change();
	
	if(armtest_flag)
		rt_mb_send (&Mb_Auto, 4);
	
	SegmentTime[2] = RunTime;
}

void armaction4(void)
{
	int flag=0;
	rt_uint32_t key_value;
	
	while(RouteFinish)
	{
		if(Point_NowNum>(Route[3]+10)&&flag==0)//尽快将自动机器人放在手动机器人上
		{
			flag=1;
			HandUD(1.5,WAIT);
			HandFB(0.8,WAIT,motor1_speed);
			HandUD(Motor2_Aim_Pos[2],WAIT);
			//HandFB(Motor1_Aim_Pos[2],WAIT);//放置自动机器人在梅花桩上
			//HandUD(Motor2_Aim_Pos[2],WAIT);
		}
		Delay_ms(5);
	}
	RouteTime[3] = RunTime;//记录第四段路径的时间
	//路径结束以后
	
	if(CircleFlag[Circle_pw])
	{
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Ready to locate");

		Wait_Button8_Change();
		
		CamCircle_Static(ARMACTION,Circle_pw);
	}
	else 
	{
// 	  if(RecodeFlag)
//       CamRecode(Recode_Circle,Circle_pw,ARMACTION);
	}
	
	if(WalkGround)
	{
		CamRecode(Recode_Pole,NONE,ARMACTION);
	
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-PW-Put");
			LCD_SetXY(0,1);LCD_WriteString("Find a place camera can see");
			LCD_SetXY(0,2);LCD_Printf("X:%f",GPS.position.x);
			LCD_SetXY(0,3);LCD_Printf("Y:%f",GPS.position.y);
			LCD_SetXY(0,4);LCD_Printf("A:%f",GPS.radian);
			LCD_SetXY(0,5);LCD_Printf("M1:%f",Motor_Pos[0]);
            Delay_ms(5);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		GPS_Record[3] = GPS;
		Motor1_Aim_Pos[2] = Motor_Pos[0];
		
	}
	
	if(debug)
		Wait_Button8_Change();

	if(Retry)
	{
		Retry  = 0;
		HandUD(Motor2_Aim_Pos[2],WAIT);
	}
	HandFB(Motor1_Aim_Pos[2],WAIT,motor1_speed);//放置自动机器人在梅花桩上
	
	if(PoleFlag)
	{
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Ready to put the AR.");
		
		Wait_Button8_Change();
		
		PolePut(ARMACTION);
	}
    else if(LaserFlag[1])
	{
		Wait_Button8_Change();
		Speed_Y = -100;
		Wait_Button8_Change();
		SPEED_STOP;
		Hall_Send(3);//夹第一个盘子
	}
	
// 	LCD_Clear();
// 	LCD_SetXY(0,1);
// 	LCD_Printf("Press button 8 to give sugar");
	
// 	Wait_Button8_Change();
// 	
// 	Hall_Send(3);//夹第一个盘子
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Press button 8 to open hand");
	Wait_Button8_Change();
	
	HandOpen();
	Delay_ms(50);
	
	HandUD(1.5,NO);
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_WriteString("arm4 done!");
  	
	LCD_SetXY(0,2);
	LCD_Printf("Time:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);
	
	//Wait_Button8_Change();
	if(armtest_flag)
		rt_mb_send (&Mb_Auto, 5);
	
	SegmentTime[3] = RunTime;
}

void armaction5(void)
{
	int flag=0;
	rt_uint32_t key_value;
	
	while(RouteFinish)
	{
		if(Point_NowNum>(Route[4]+10)&&flag==0)//尽快将自动机器人放在手动机器人上
		{
			flag=1;
			HandUD(1.5,NO);
		}
		Delay_ms(5);
	}
	RouteTime[4] = RunTime;//记录第五段路径的时间
	if(WalkGround)
	{
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-PW-Get");
			LCD_SetXY(0,1);LCD_WriteString("Find a place camera can see");
			LCD_SetXY(0,2);LCD_Printf("X:%f",GPS.position.x);
			LCD_SetXY(0,3);LCD_Printf("Y:%f",GPS.position.y);
			LCD_SetXY(0,4);LCD_Printf("A:%f",GPS.radian);
			LCD_SetXY(0,5);LCD_Printf("M1:%f",Motor_Pos[0]);
            Delay_ms(5);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		GPS_Record[4] = GPS;
		Motor1_Aim_Pos[3] = Motor_Pos[0];
		
		CamRecode(Recode_Handover,Aim_pw,ARMACTION);
	}
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_WriteString("Ready to handover");
	
	Wait_Button8_Change();
	
	if(HandoverFlag[Aim_pw])//是否要在梅花桩交接的标志位
    {
        HandUD(Motor2_Aim_Pos[3],WAIT);
        Handover(ARMACTION,Aim_pw);
    }
	else//手动交接，手动抓取，手动喂糖
	{
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Ready to got the child?");
		
		Wait_Button8_Change();
		HandUD(Motor2_Aim_Pos[3]+1.5f,NO);
	}
	
	while(CHILD_OFF)//判断是否抓到自动车
		Delay_ms(5);
	Moto_Stop(W_MOTOR2_OLD_ID);
	HandClose();
	Delay_ms(50);
// 	Delay_ms(50);
// 	Hall_Send(4);//松盘子 & 秋千初始化(SW)
// 	Delay_ms(50);
// 	Hall_Send(4);//此处发两次跟上面同样原因
// 	Delay_ms(50);
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_WriteString("arm5 done!");
	
	LCD_SetXY(0,2);
	LCD_Printf("Time:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);	
	
	if(debug)
		Wait_Button8_Change();
	
	if(armtest_flag)
		rt_mb_send (&Mb_Auto, 6);
	
	SegmentTime[4] = RunTime;
}

void armaction6(void)
{
 	struct Point temp;
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	u8 delay_flag = 1;
	extern u8 fan_flag;
	rt_uint32_t key_value;
	
	if(armtest_flag && !Retry)
	{
		HandFB(1,WAIT,motor1_speed);
		HandUD(1.78,WAIT);
	}
	
	Temp_button8_state = Button8_State;
	
	while(RouteFinish)
	{
		if(Temp_button8_state != Button8_State)
		{
				Auto_flag = 0;
				Temp_button8_state = Button8_State;
		}
		if(Point_NowNum>(Route[5]+200)&&flag1==0)
		{
			flag1=1;
			HandTurnRight();
			Delay_ms(600);
			HandFB(0,NO,motor1_speed);
			HandUD(3,NO);
			GunOn();//大炮准备
		}
		if(Point_NowNum>(Route[7]-400)&&flag2==0)
		{
			flag2=1;
			fan_flag = 0;
// 			Fan_Duty(L_CH,60.0);
// 			Fan_Duty(R_CH,60.0);
		}
		if(Point_NowNum>(Route[7]-200)&&flag3==0)
		{
			flag3=1;
			HandUD(0,NO);
		}
		Delay_ms(5);
	}
	Retry = 0;
	fan_flag = 1;
	RouteTime[5] = RunTime;//记录第六段路径的时间
	if(WalkGround)
		CamRecode(Recode_WhiteTape,NONE,ARMACTION);
	
	if(Auto_flag)
		Wait_Button8_Change();
	Auto_flag = 1;
	
	if(SwingFlag)
	{
		ChooseFunction(Detect_WhiteTape);
		
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Ready to locate");
        
		CamWhiteTape(ARMACTION);
	}
	else
	{
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("Ready to place the child");
		//Wait_Button8_Change();
		HandFB(Motor1_Aim_Pos[4],WAIT,motor1_speed);//放置自动机器人
	}
	
	Delay_ms(100);
	HandUD(Motor2_Aim_Pos[4],WAIT);
	
	if(WalkGround)
	{
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-SW-Put");
			LCD_SetXY(0,1);LCD_Printf("X:%f",GPS.position.x);
			LCD_SetXY(0,2);LCD_Printf("Y:%f",GPS.position.y);
			LCD_SetXY(0,3);LCD_Printf("A:%f",GPS.radian);
			LCD_SetXY(0,4);LCD_Printf("M1:%f",Motor_Pos[0]);
			LCD_SetXY(0,5);LCD_Printf("M2:%f",Motor_Pos[1]);
            Delay_ms(5);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		GPS_Record[5] = GPS;
		Motor1_Aim_Pos[4] = Motor_Pos[0];
		Motor2_Aim_Pos[4] = Motor_Pos[1];
	}
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to open hand");
	Wait_Button8_Change();
	
	HandOpen();//松开爪子
	while(CHILD_ON)//判断自动车是否脱离
		Delay_ms(5);
	
	HandUD(0,WAIT);
	HandClose();//关爪子
	
	HandFB(0,NO,motor1_speed);
	
	if(debug)
		Wait_Button8_Change();
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to fire");
	//Delay_ms(300);
	Wait_Button8_Change();
	FireOn();//开炮
	Delay_ms(100);
	
	if(!armtest_flag)
	{
		GPS_Init(GPS.position,45.0/180.0*pi);
		Delay_ms(20);
		temp.x = GPS.position.x-700*cos(GPS.radian);
		temp.y = GPS.position.y-700*sin(GPS.radian);
	}
	else
	{
		temp.x = GPS.position.x-700*cos(GPS.radian);
		temp.y = GPS.position.y-700*sin(GPS.radian);
		//temp.x = PointRoute[Route[6]].position.x-700;
		//temp.y = PointRoute[Route[6]].position.y-700;
	}
	
	SetLine(temp,GPS.radian,200,800,0);
	while(1)
	{
		GoLine();
		if(delay_flag)
			delay_flag++;
		if(delay_flag>100)
		{
			delay_flag = 0;
			FireOff();
		}
		if(GetLength(GPS.position,temp) < 10)
		{
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
    
	FireClear();
	
	Delay_ms(50);
	BufferOn();//打开缓冲装置
	Delay_ms(500);
	BufferOff();//重新复位缓冲装置
    
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to stop the SW");
	
	Wait_Button8_Change();
	
	HandFB(Motor1_Aim_Pos[4],NO,motor1_speed);//抓取自动机器人
	temp.x = GPS.position.x+680*cos(GPS.radian);
	temp.y = GPS.position.y+680*sin(GPS.radian);
	SetLine(temp,GPS.radian,200,800,0);
	
	while(1)
	{
		GoLine();
		if(GetLength(GPS.position,temp) < 10)
		{
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
	
	HandUD(0.5,NO);
	HandOpen();//松开爪子
	
	Delay_ms(300);
	FireOff();
	BufferOn();//打开缓冲装置
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to go to catch the child");
	Wait_Button8_Change();

//	if(WalkGround)
//		CamRecode(Recode_Handover,Aim_sw,ARMACTION);
//	
//	if(HandoverFlag[Aim_sw])
//		Handover(ARMACTION,Aim_sw);
	
	HandUD(1.25,NO);
	
	while(CHILD_OFF)
		Delay_ms(5);
	Moto_Stop(W_MOTOR2_OLD_ID);
	
	HandClose();
 	Delay_ms(100);
// 	Hall_Send(5);//松开秋千
// 	Delay_ms(50);
// 	Hall_Send(5);
// 	Delay_ms(50);
	
	HandUD(0,WAIT);
	
	HandFB(0,NO,5000);
	
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to go");
	
	LCD_SetXY(0,2);
	LCD_Printf("Time:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);
	
	Wait_Button8_Change();
	
//	SetLine(PointRoute[Route[8]].position,PointRoute[Route[8]].selfdir,900,900,900);
//	while(1)
//	{
//		GoLine();
//		if(GetLength(GPS.position,temp) < 10)
//		{
//			break;
//		}
//		Delay_ms(5);
//	}
	
	if(armtest_flag)
		rt_mb_send (&Mb_Auto, 7);
	
	SegmentTime[5] = RunTime;
}

void armaction7(void)
{
	int flag=0;
	int i;
	rt_uint32_t key_value;
	struct Point temp;
	
	GunOff();//收回大炮
	
	while(RouteFinish)
	{
		if(Point_NowNum>(Route[8]+350)&&flag==0)
		{
			flag=1;
	// 		Hall_Send(6);//爬楼梯初始化 & 抓楼梯
			HandUD(1,WAIT);
			HandRaise();//俯仰调姿
			Delay_ms(600);
			HandFB(1,WAIT,motor1_speed);
			//W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID,3.0/4.0*Motor2_Aim_Pos[5], motor1_speed , CMD_SET_PSG);
		}
		Delay_ms(10);
	}
	
	Retry = 0;
	RouteTime[6] = RunTime;//记录第七段路径的时间
	
	if(WalkGround)
	{
		CamRecode(Recode_Circle,Circle_jg,ARMACTION);
		
		if(CircleFlag[Circle_jg])
		{
			Wait_Button8_Change();
			CamCircle_Static(ARMACTION,Circle_jg);
		}
		
		while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
		{
			LCD_Clear();
			LCD_SetXY(0,0);LCD_WriteString("WalkGround-JG-Put");
			LCD_SetXY(0,1);LCD_Printf("X:%f",GPS.position.x);
			LCD_SetXY(0,2);LCD_Printf("Y:%f",GPS.position.y);
			LCD_SetXY(0,3);LCD_Printf("A:%f",GPS.radian);
			LCD_SetXY(0,4);LCD_Printf("M1:%f",Motor_Pos[0]);
			LCD_SetXY(0,5);LCD_Printf("M2:%f",Motor_Pos[1]);
			Delay_ms(5);
		}
		LCD_Clear();
		LCD_SetXY(0,0);LCD_WriteString("Done");
		GPS_Record[6] = GPS;
		Motor1_Aim_Pos[5] = Motor_Pos[0];
		Motor2_Aim_Pos[5] = Motor_Pos[1];
	}
	
	HandUD(Motor2_Aim_Pos[5],WAIT);
	HandFB(Motor1_Aim_Pos[5],WAIT,motor1_speed);
	LCD_Clear();
	LCD_SetXY(0,1);
	LCD_Printf("Ready to open hand");
	Wait_Button8_Change();
	
	HandOpen();//松开爪子
	Delay_ms(50);
	HandFB(0,NO,5000);
	Delay_ms(200);
    temp.x = GPS.position.x+580*sin(GPS.radian);
	temp.y = GPS.position.y-580*cos(GPS.radian);
	
	SetLine(temp,GPS.radian,1000,1000,0);
	while(1)
	{
		GoLine();
		if(GetLength(GPS.position,temp) < 10)
		{
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
	
	Temp_button8_state = Button8_State;
	
	while(Temp_button8_state==Button8_State)
	{
		LCD_Clear();
		LCD_SetXY(0,2);
		LCD_Printf("Time:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);		
		Delay_ms(5);
	}
	
	TimeOnFlag = 0;
	SegmentTime[6] = RunTime;
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_Printf("TotalTime:%d min %d s",RunTime*5/1000/60,RunTime*5/1000%60);		
	LCD_SetXY(0,1);
	LCD_Printf("arm1 time:%d min %d s",SegmentTime[0]*5/1000/60,SegmentTime[0]*5/1000%60);		 	
	for(i=0;i<6;i++)
	{
	  LCD_SetXY(0,i+2);
	  LCD_Printf("arm%d time:%d min %d s",i+2,(SegmentTime[i+1]-SegmentTime[i])*5/1000/60,(SegmentTime[i+1]-SegmentTime[i])*5/1000%60);	
	}
	
	SD_Printf_MQ(&cam_file,"THE END");
	
	SD_Printf_MQ(&fil_pid,"\r\nRoute1:%f\tArmaction1:%f",RouteTime[0]*0.005,(SegmentTime[0]-RouteTime[0])*0.005);
	for(i=1;i<7;i++)
	{
		SD_Printf_MQ(&fil_pid,"\r\nRoute%d:%f\tArmaction%d:%f",i+1,(RouteTime[i]-SegmentTime[i-1])*0.005,i+1,(SegmentTime[i]-RouteTime[i])*0.005);
	}
	SD_Printf_MQ(&fil_pid,"\r\nRouteTime:%f\r\n",
	(RouteTime[0]
	+RouteTime[1]-SegmentTime[0]
	+RouteTime[2]-SegmentTime[1]
	+RouteTime[3]-SegmentTime[2]
	+RouteTime[4]-SegmentTime[3]
	+RouteTime[5]-SegmentTime[4]
	+RouteTime[6]-SegmentTime[5])*0.005);
	SD_Printf_MQ(&fil_pid,"All Time:%f\r\n",SegmentTime[6]*0.005);
	SD_Printf_MQ(&fil_pid,"THE END");
	
	if(WalkGround)
	{
		SD_Walkground();
		
		LCD_Clear();
		for(i=0;i<7;i++)
		{
			LCD_SetXY(0,i);
			LCD_Printf("%f",GPS_Record[i].position.x,GPS_Record[i].position.y,GPS_Record[i].radian);
			LCD_SetXY(6,i);
			LCD_Printf(" %f",GPS_Record[i].position.y);
			LCD_SetXY(13,i);
			LCD_Printf(" %f",GPS_Record[i].radian);	
		}
	}
}

/**********************************END OF FILE*********************************/
