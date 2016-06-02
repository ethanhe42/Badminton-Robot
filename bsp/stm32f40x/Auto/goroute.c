
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "PointRoute.h"
#include "math.h"

#define motor1_speed 1800 

u8 Route_Num=1;
int Route[10]={0,818,1619,2420,3091,3189,3994,4641,4704,5400};

u8 RouteFinish;//对其置零即可使路径停止
u8 CircleFinish = 1;
int SwitchNum = 40;

extern u8 CircleFlag[4];
extern u8 LaserFlag[];
extern double acc_p;
extern double V_max;
extern double acc_n;
extern double CAM_R;
extern u8 Circle_dynFlag;
extern u8 SwingFlag;
extern u8 WalkGround;

extern struct Path_Point PointRoute[];

/*
 * 函数名: goroute1
 * 描  述: 走第一段路径
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void GoRoute1(void)//从出发位置到跷跷板处
{
	SD_ReInit();
	
	Route_Num=1;
	SetPointPath(Route[0],Route[1]);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	while(1)
	{
		GoPointPath();
		
		if(Point_NowNum>Point_EndNum-2)
		{
			RouteFinish=0;
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
}

void GoRoute2(void)//从跷跷板一端到跷跷板另一端
{
	u8 acc_flag = 0;
	u8 flag1 = 0;
	u8 flag2 = 0;
	int time_interval = 5;
	Route_Num = 2;
	SetPointPath(Route[1]+1,Route[2]);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	if(Circle_dynFlag)
	{
     ChooseFunction(Detect_Circle);			
		 while(1)
		 {
			 Go_CamCircle();
			 Delay_ms(time_interval);
			 if(!acc_flag)
				 Speed_Y += time_interval*acc_p/1000;
			 if(Speed_Y>V_max)
				 Speed_Y = V_max;
			 if(Gyro.angle<(-135+30)&&flag1==0)
			 {
				 flag1 = 1;
				 acc_flag = 1;
			 }
			 if(Gyro.angle<=-135&&flag2==0)
			 {
				 flag2 = 1;
				 SPEED_STOP;
			 }
			 if(acc_flag)
				 Speed_Y -= time_interval*acc_n/1000;
			 if(Speed_Y<=0)
			 {
				 Speed_Y = 0;
				 acc_flag = 0;
			 }
			 if(Speed_Y==0&&Abs(Aim_Dist-Cam_Dist[Cam_Circle_Line])<5&&Abs(Aim_Angle-Cam_Angle[Cam_Circle_Line])/180.0*pi<0.03)
			 {
				 CircleFinish = 0;
				 break;
			 }
		 }
	 
	   SPEED_STOP;
	}
	else
	{
		while(1)
		{
			GoPointPath();
			if(LaserFlag[0])
			{
				if(Point_NowNum>1550)
				{
					//lssp(1);
					RouteFinish=0;
					SPEED_STOP;
					break;
				}
			}
			else
			{
				if(!WalkGround&&CircleFlag[0])
				{
					ChooseFunction(Detect_Circle);
					if(Point_NowNum>Point_EndNum-SwitchNum&&camera_data1.time_flag<100)
					{
						RouteFinish = 0;
						break;
					}
				}
				if(Point_NowNum>Point_EndNum-2)
				{
					SPEED_STOP;
					
//					#if   GROUND==RED_GROUND
//						SetKeep(PointRoute[Route[2]].position,pi/4,300);
//					#elif GROUND==BLUE_GROUND
//						SetKeep(PointRoute[Route[2]].position,-3.0/4.0*pi,300);
//					#endif
					
					RouteFinish=0;
					break;
				}
			}
			Delay_ms(5);
		}
		if(!WalkGround&&CircleFlag[Circle_ss1])
			CamCircle_Static(ARMACTION,Circle_ss1);
	}
}

void GoRoute3(void)//从跷跷板另一端再回来
{
	Route_Num=3;
	SetPointPath(Route[2],Route[3]);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	while(1)
	{
		GoPointPath();
		if(Point_NowNum>Point_EndNum-2)
		{
			RouteFinish=0;
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
}
/*   带激光测距的，路径要改
void GoRoute4(void)//从跷跷板到梅花桩处
{
	u8 flag = 1;
	Route_Num=4;
	SetPointPath(Route[3]+1,Route[4]);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	while(RouteFinish)
	{
		GoPointPath();
		
		if(Point_NowNum>Point_EndNum-2)
		{
			if(LaserFlag[1] && flag)
			{
				if(ls2p(e2p_pw)==0)
				{
					RouteFinish=0;
					SPEED_STOP;
					break;
				}
			}
			if(!flag)
			{
				RouteFinish=0;
				SPEED_STOP;
				break;
			}
			if(flag)
			{
				flag = 0;
				SetPointPath(Route[4],Route[4]+250);
			}
		}
		Delay_ms(5);
	}
}
*/

void GoRoute4(void)//从跷跷板到梅花桩处
{
	rt_uint32_t msg;
	u8 flag = 1;
	Route_Num=4;
	SetPointPath(Route[3]+1,Route[4]);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	while(RouteFinish)
	{
		GoPointPath();
		
		if(Point_NowNum>Point_EndNum-200 && LaserFlag[1] && flag)
		{
			flag = 0;
			rt_mb_send (&Mb_SensorStart, eLspw_start);
		}
		
		if(rt_mb_recv(&Mb_SensorEnd, &msg, RT_WAITING_NO) == RT_EOK)
			if(msg == eLspw_find)
			{
				if(rt_mb_recv(&Mb_SensorEnd, &msg, RT_WAITING_FOREVER) == RT_EOK)
				{
					if(msg == eLspw_end)
					{
						RouteFinish=0;
						SPEED_STOP;
						break;
					}
				}
			}
		
		if(Point_NowNum>Point_EndNum-2)
		{
			RouteFinish=0;
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
}


//void GoRoute5(void)//从梅花桩一端到另一端
//{
//	Route_Num=5;
//	SetPointPath(Route[4]+1,Route[5]);
//	rt_mb_send (&Mb_Arm, Route_Num);
//	
//	while(RouteFinish)
//	{
//		GoPointPath();
//		if(Point_NowNum>Point_EndNum-2)
//		{
//			RouteFinish=0;
//			SPEED_STOP;
//			break;
//		}
//		Delay_ms(5);
//	}
//	
//	LCD_Clear();
//	LCD_SetXY(0,1);
//	LCD_Printf("have sent the mail!");
//}

void GoRoute5(void)//从梅花桩一端到另一端
{
	struct Point temp;
	Route_Num=5;
	RouteFinish=1;
	Delay_ms(50);
	
	if(GROUND == RED)
	{
		temp.x = GPS.position.x+580*sin(GPS.radian);
		temp.y = GPS.position.y-580*cos(GPS.radian);
	}
	else if(GROUND == BLUE)
	{
		temp.x = GPS.position.x-580*sin(GPS.radian);
		temp.y = GPS.position.y+580*cos(GPS.radian);
	}
	
	SetLine(temp,GPS.radian,200,800,0);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	
	while(RouteFinish)
	{
		GoLine();
		if(GetLength(GPS.position,temp) < 10)
		{
			RouteFinish=0;
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
}


void GoRoute6(void)//从梅花桩到秋千处
{
	Route_Num=6;
	SetPointPath(Route[5]+1,Route[7]);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	
	while(1)
	{
		GoPointPath();
		if(Point_NowNum>Point_EndNum-2)
		{
			RouteFinish = 0;
			SPEED_STOP;
			break;
		}
		Delay_ms(5);
	}
}

void GoRoute7(void)//从秋千到楼梯处
{
	rt_uint32_t msg;
	u8 flag = 1;
	Route_Num=7;
	SetPointPath(Route[8]+1,Route[9]);
	
	rt_mb_send (&Mb_Arm, Route_Num);
	
	while(RouteFinish)
	{
		GoPointPath();
		
		if(Point_NowNum>Point_EndNum-200 && LaserFlag[2] && flag)
		{
			flag = 0;
			rt_mb_send (&Mb_SensorStart, eLsld_start);
		}
		
		if(rt_mb_recv(&Mb_SensorEnd, &msg, RT_WAITING_NO) == RT_EOK)
			if(msg == eLsld_find)
			{
				if(rt_mb_recv(&Mb_SensorEnd, &msg, RT_WAITING_FOREVER) == RT_EOK)
				{
					if(msg == eLsld_end)
					{
						RouteFinish=0;
						SPEED_STOP;
						break;
					}
				}
			}
			
		if(!WalkGround&&CircleFlag[Circle_jg])
		{
			ChooseFunction(Detect_Circle);
			if(Point_NowNum>Point_EndNum-SwitchNum&&camera_data1.time_flag<100)
			{
				RouteFinish = 0;
				break;
			}
		}
		if(Point_NowNum>Point_EndNum-2)
		{
			RouteFinish=0;
			SPEED_STOP;
			break;
		}
		
		Delay_ms(5);
	}
	if(!WalkGround&&CircleFlag[Circle_jg])
		CamCircle_Static(ARMACTION,Circle_jg);
}


/**********************************END OF FILE*********************************/
