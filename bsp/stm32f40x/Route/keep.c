/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"

extern FIL cam_file;

s16 Aim_X_Handover[4];
s16 Aim_Y_Handover[4];
s16 Aim_Z_Handover[4];//分别为在跷跷板、梅花桩以及秋千处交接的理想坐标,在t_keyboard.c里面可以进行初始化
s16 Aim_A_Handover[4];

s16 Aim_X_Pole;//在梅花桩处放置自动机器人的理想坐标
s16 Aim_Y_Pole;
fp64 Aim_A_Pole;

fp64 Aim_D_Swing[3];//   在秋千处的理想坐标
fp64 Aim_A_Swing[2];//46

fp64 Aim_D_Circle[4];
fp64 Aim_A_Circle[4];//在楼梯、梅花桩和跷跷板等处靠圆定位的理想坐标
fp64 Aim_A_G_Circle[4];

struct Point Aim_Keep_Point;
double Aim_Keep_Radian;
double Speed_Keep_Max;

fp64 SW_SPEED = 400;

double Aim_Angle;   //走圆的目标位置
double Aim_Dist;

double V_Max_Z[2] = {4000.0,2000.0};//交接上下电机最大的速度
double Handover_R = -500.0;
double PolePut_R;

u8 HandoverFinish;
u8 PoleFinish;
u8 SwingFinish;
u8 Cam_F_DISABLE;
u8 Handover_Z_DISABLE;
u8 Cannot_See_Flag;
u8 Stop_Flag[3];
extern u8 handle_timeout_flag;
extern s16 max_e_x;
extern double SPEED_DOWN;
extern u8 REDFlag;

extern double stop_ed_static;
extern double stop_ea_static;

/*
 * 函数名: SetKeep
 * 描  述: 快速到达指定位置与角度
 * 输  入: -end_point  目标位置
 *		   -aim_radian 目标角度
 * 输  出: 无
 * 调  用: 外部调用
 */
void SetKeep(struct Point end_point,double aim_radian,double speed_max)
{
	Aim_Keep_Point = end_point;
	Aim_Keep_Radian = aim_radian;
	Speed_Keep_Max = speed_max;
}

void Keep(void)
{
		double lineangle;
	
		double speed = 0;
		double speed_x,speed_y;						//世界坐标里的速度与分速度
		double speed_robot_x,speed_robot_y;			//转换到机器人坐标的分速度
		double phi;
	
		double error_angle,error_dis;				//角度误差与距离误差
		double Vout_A = 0;							//PID调整后角度输出值
		double Vout_D = 0;							//PID调整后距离输出值
		lineangle = GetLineAngle(GPS.position,Aim_Keep_Point);	//求出该路径线段的倾角
		
		//求角度误差与距离误差
		error_angle = Aim_Keep_Radian-GPS.radian;
		error_dis = GetLength(GPS.position,Aim_Keep_Point);
		
		if(error_angle>pi)
			error_angle-=2*pi;
		if(error_angle<-pi)
			error_angle+=2*pi;
		
		//PID调整
		Vout_A = 1000*AnglePID(error_angle,Pid_List[4]);
		Vout_D = DistancePID(error_dis,Pid_List[5]);
		
		//限最大转速
		if(Vout_A > 300) Vout_A = 300;
		else if(Vout_A < -300) Vout_A = -300;
		
		speed = Vout_D;
		
		//限最大速
		if(speed > Speed_Keep_Max) speed = Speed_Keep_Max;
		else if(speed < -Speed_Keep_Max) speed = -Speed_Keep_Max;
		
		//求速度在x,y方向上的分量
		speed_x = speed*cos(lineangle);
		speed_y = speed*sin(lineangle);
		
		//将世界坐标的速度转换为机器人局部坐标的速度
		phi = GPS.radian;
		speed_robot_x =  speed_x*cos(phi) + speed_y*sin(phi);
		speed_robot_y = -speed_x*sin(phi) + speed_y*cos(phi);
		
		//为四轮赋速度
		
		Speed_X = speed_robot_x;
		Speed_Y = speed_robot_y;
		Speed_Rotation = Vout_A;
		
		//LCD显示
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("A:");
		LCD_WriteDouble(GPS.radian);
		
		LCD_SetXY(0,1);
		LCD_WriteString("X:");
		LCD_WriteDouble(GPS.position.x);
		
		LCD_SetXY(9,1);
		LCD_WriteString(" Y:");
		LCD_WriteDouble(GPS.position.y);
		
		LCD_SetXY(0,2);
		LCD_WriteString("error_angle:");
		LCD_WriteDouble(error_angle);
		
		LCD_SetXY(0,3);
		LCD_WriteString("error_dis:");
		LCD_WriteDouble(error_dis);
		
		LCD_SetXY(0,4);
		LCD_WriteString("Vout_A:");
		LCD_WriteDouble(Vout_A);
		
		LCD_SetXY(0,5);
		LCD_WriteString("Vout_D:");
		LCD_WriteDouble(Vout_D);
		
		LCD_SetXY(0,6);
		LCD_WriteDouble(speed_x);
		LCD_SetXY(10,6);
		LCD_WriteDouble(speed_y);
		
		LCD_SetXY(0,7);
		LCD_WriteDouble(lineangle);	
}

void SetHandover(s16 aim_x,s16 aim_y,s16 aim_z,s16 aim_a,int place)
{
	HandoverFinish=1;
	Aim_X_Handover[place] = aim_x;
	Aim_Y_Handover[place] = aim_y;
	Aim_Z_Handover[place] = aim_z;
	Aim_A_Handover[place] = aim_a;
}

#define MAX_SPEED 2000
void Keep_Handover(int place)
{
	s16 error_x,error_y,error_z;
	static u8 flag;
	s16 error_a;
	
	error_x = Cam_X-Aim_X_Handover[place];
	error_y = Cam_Y-Aim_Y_Handover[place];
	error_z = Cam_Z-Aim_Z_Handover[place];
	error_a = Cam_A-Aim_A_Handover[place];
	
	if(error_a>180)
		error_a -= 360;
	if(error_a<-180)
		error_a += 360;

  LCD_Clear();
	if(handle_timeout_flag)
	{
		SPEED_STOP;
		LCD_Printf("Handle Gone!");
	}	
	
	else
	{
		if(camera_data5.time_flag<100)
		{
				flag = 0;
				Buz_Off();
				CamLED_Off();
				Cannot_See_Flag  = 0;
				if((place == Aim_ss) || (place == Aim_pw) || (place == Aim_sw))
				{
						if((place == Aim_ss)||(place == Aim_pw))
						{											
							if(Abs(error_y)<5)
								Vout[2] = 0;						
							else 
								Vout[2] = Cam_Y_PID(error_y,Pid_List[14]);								
							
							if(Vout[2]>MAX_SPEED)
								Vout[2] = MAX_SPEED;
							if(Vout[2]<-MAX_SPEED)
								Vout[2] = -MAX_SPEED;						
							
							if(Abs(error_x)<5)
								Vout[4] = 0;
							else						
								Vout[4] = Cam_X_PID(error_x,Pid_List[15]);						
							
							
							if(Abs(error_z)<5||SA_READ(3)==0||SA_READ(4)==0)
								Vout[3] = 0;
							else							
								Vout[3] = Cam_Z_PID(error_z,Pid_List[16]);
							
							if(Vout[3]>V_Max_Z[0])
								Vout[3] = V_Max_Z[0];
							if(Vout[3]<-V_Max_Z[0])
								Vout[3] = -V_Max_Z[0];
						}
						
						else 
						{
							if(Abs(error_y)<5)
								Vout[4] = 0;
							else
								Vout[4] = Cam_Y_PID(error_y,Pid_List[24]);
									
							if(Abs(error_x)<5)
								Vout[2] = 0;	
							else 
								Vout[2] = Cam_X_PID(error_x,Pid_List[23]);
							
							if(Vout[2] > MAX_SPEED)
								Vout[2] = MAX_SPEED;
							if(Vout[2] < -MAX_SPEED)
								Vout[2] = -MAX_SPEED;
							
							if(Abs(error_z)<5||SA_READ(3)==0||SA_READ(4)==0)
								Vout[3] = 0;		
							Vout[3] = Cam_Z_PID(error_z,Pid_List[25]);
							
							if(Vout[3] > V_Max_Z[1])
								Vout[3] = V_Max_Z[1];
							if(Vout[3] < -V_Max_Z[1])
								Vout[3] = -V_Max_Z[1];
								
						}
						
						if(Abs(error_a)<4)
							Vout[1] = 0;
						else
							Vout[1] = Cam_A_PID(error_a,Pid_List[17]);
							 
					
					 
						Moto_Set_GSpeed(W_MOTOR1_OLD_ID,Vout[2]);
						
						if(!Handover_Z_DISABLE)
							Moto_Set_GSpeed(W_MOTOR2_OLD_ID,Vout[3]);							
						
						Speed_Y = Vout[4]+Vout[1];
						Speed_X = 0;
						Speed_Rotation = 100*Vout[1]/Handover_R;
					
						LCD_Clear();
						LCD_SetXY(0,0);
						LCD_Printf("c_x:%d  e_x:%d",Cam_X,error_x);
						LCD_SetXY(0,1);
						LCD_Printf("c_y:%d  e_y:%d",Cam_Y,error_y);
						LCD_SetXY(0,2);
						LCD_Printf("c_z:%d  e_z:%d",Cam_Z,error_z);
						LCD_SetXY(0,3);
						LCD_Printf("c_a:%d  e_a:%d",Cam_A,error_a);
						LCD_SetXY(0,4);
						LCD_Printf("Vout_UP:%.2f",Vout[2]);
						LCD_SetXY(0,5);
						LCD_Printf("Vout_DOWN:%.2f",Speed_Y);		
						LCD_SetXY(0,6);
						LCD_Printf("Vout_z:%.2f",Vout[3]);
						LCD_SetXY(0,7);
						LCD_Printf("V_r:%.2f",Speed_Rotation);						
				}
				else
				{
					 SPEED_STOP;
					 Moto_Stop(W_MOTOR2_OLD_ID); 
					 Moto_Stop(W_MOTOR1_OLD_ID); 
					 LCD_Clear();
					 LCD_SetXY(0,1);
					 LCD_Printf("place error!");
				}

	// 		}
		}
		else //摄像头长时间没有收到数据
		{	
			Buz_On();
			CamLED_Toggle();
			Cannot_See_Flag  = 1;
			if (!flag)//只给底盘电机和上层电机赋一次速度为零
			{
				SPEED_STOP;
				Moto_Stop(W_MOTOR1_OLD_ID); 
				Moto_Stop(W_MOTOR2_OLD_ID);
				flag = 1;
			}
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Long time data not received!");
		}
	}

	SD_Printf_MQ(&cam_file,"%d %d %d %d %.5f %.5f %.5f\r\n",Cam_X,Cam_Y,Cam_Z,Cam_A,Speed_Y,Speed_Rotation,Vout[2]);	
		//输出数据，for 张百齐， by冯新华
}

void SetPole(s16 aim_x,s16 aim_y,fp64 aim_a)
{
	PoleFinish = 1;
	Aim_X_Pole = aim_x;
	Aim_Y_Pole = aim_y;
	Aim_A_Pole = aim_a;
}

void Keep_Pole(void)
{
	s16 error_x,error_y;
	double error_angle;
	static u8 flag;
	
	error_x = Cam_Pole_X - Aim_X_Pole;
	error_y = Cam_Pole_Y - Aim_Y_Pole;
	error_angle = Gyro.angle - Aim_A_Pole;
	
	if(error_angle>180)
		error_angle = error_angle - 360;
	if(error_angle<-180)
		error_angle = error_angle + 360;	
	
	LCD_Clear();
	if(handle_timeout_flag)
	{
		SPEED_STOP;
		LCD_Printf("Handle Gone!");
	}
	else
	{
		if(camera_data4.time_flag<100)
		{
			flag = 0;
			Buz_Off();
			CamLED_Off();
			Cannot_See_Flag = 0;
			if(Abs(error_x)<=1&&Abs(error_angle)<0.015)
				SPEED_STOP;
			else
			{
				if(error_angle<0.02&&Abs(error_y)<20)
				  Vout[0] =	Cam_Y_PID(error_x,Pid_List[18]);
				
        else				
					Vout[0] = 0;
				
				Speed_X = 0;
				
// 				Vout[1] = AnglePID(error_angle,Pid_List[20]);	
// 				Speed_Rotation = Vout[1]/PolePut_R;
				Speed_Rotation = 1000*AnglePID(error_angle,Pid_List[20]);
// 				Speed_Y = Vout[1]+Vout[0];
				Speed_Y = Vout[0];
				if(Speed_Rotation>300)
					Speed_Rotation=300;
				if(Speed_Rotation<-300)
					Speed_Rotation=-300;
			}
			
			if(Abs(error_y)<=1||error_angle>0.02)
				Vout[2] = 0;
			else{
				Vout[2] = Cam_X_PID(error_y,Pid_List[19]);
				if(Vout[2]>2500)
					Vout[2]=2500;
				if(Vout[2]<-2500)
					Vout[2]=-2500;
			} 
			Moto_Set_GSpeed(W_MOTOR1_OLD_ID,Vout[2]);
			
			LCD_Clear();
			LCD_SetXY(0,0);
			LCD_Printf("c_x:%d  e_x:%d",Cam_Pole_X,error_x);
			LCD_SetXY(0,1);
			LCD_Printf("c_y:%d  e_y:%d",Cam_Pole_Y,error_y);
			LCD_SetXY(0,2);
			LCD_WriteString("a:");
			LCD_WriteNum(Gyro.angle);
			LCD_SetXY(10,2);
			LCD_WriteString("e_a:");
			LCD_WriteNum(error_angle/pi*180.0);
			LCD_SetXY(0,3);
			LCD_Printf("Vout_x:%.2f",Speed_Y);
			LCD_SetXY(0,4);
			LCD_Printf("Vout_y:%.2f",Vout[2]);	
			LCD_SetXY(0,5);
			LCD_WriteString("Vout_A:");
			LCD_WriteNum(Speed_Rotation);
		}
		else 
		{
			Cannot_See_Flag = 1;
			Buz_On();
			CamLED_Toggle();
			if(!flag)//只给底盘电机和上层电机赋一次速度为零
			{
				flag = 1;
				SPEED_STOP;
				Moto_Stop(W_MOTOR1_OLD_ID); 
			}
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Long time data not received!");		
		}
	}	
	
  SD_Printf_MQ(&cam_file,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\r\n",Cam_Pole_X,Cam_Pole_Y,Gyro.angle,Motor_Pos[0],Encoder[0].dis,Vout[2],Speed_Y,Speed_Rotation,GPS.position.x,GPS.position.y);	
	
}


void SetSwing(fp64 aim_d1,fp64 aim_d2,fp64 aim_a,u8 ground)
{
	switch(ground)
	{
		case RED:
			Aim_D_Swing[Cam_Circle_Line] = aim_d1;
			Aim_D_Swing[Cam_Line_RED] = aim_d2;
			break;	
		case BLUE:
			Aim_D_Swing[Cam_Circle_Line] = aim_d1;
			Aim_D_Swing[Cam_Line_BLUE] = aim_d2;
			break;
	}
	Aim_A_Swing[ground] = aim_a;
}

void Keep_Swing(u8 ground)
{
  fp64 error_dis1,error_dis2,error_angle;
	LCD_Clear();
	switch(ground)
	{
		case BLUE:	    
	    error_dis2 = Cam_Dist[Cam_Line_BLUE] - Aim_D_Swing[Cam_Line_BLUE];
		  error_angle = GPS.radian - Aim_A_Swing[ground]/180.0*pi;
			if(camera_data2.time_flag<100)
			{	
				if(camera_data1.time_flag<100)
					buz_msg(0,0,0,200);
				
				if(Abs(error_dis2)<2)
					Speed_Y = 0;
				else 
					Speed_Y = WT_D2_PID(error_dis2,Pid_List[22]);
						
				if(Speed_Y>1000)
						Speed_Y = 1000;
				if(Speed_Y<-1000)
						Speed_Y = -1000;
			}
			else 
			{
				Speed_Y = SW_SPEED;
				LCD_SetXY(0,7);
				LCD_Printf("Time out Cam_S!");		
				if(camera_data1.time_flag>100)
					buz_msg(1,2,3,200);
				else
					buz_msg(1,1,2,200);
			}
		  break;
			
		case RED:
			error_dis2 = Cam_Dist[Cam_Line_RED] - Aim_D_Swing[Cam_Line_RED];
		  error_angle = GPS.radian - Aim_A_Swing[ground]/180.0*pi;
			if(camera_data3.time_flag<100)
			{	
				if(camera_data1.time_flag<100)
					buz_msg(0,0,0,200);
				if(Abs(error_dis2)<2)
					Speed_Y = 0;
				else 
					Speed_Y = WT_D2_PID(error_dis2,Pid_List[22]);
						
				if(Speed_Y>1000)
						Speed_Y = 1000;
				if(Speed_Y<-1000)
						Speed_Y = -1000;
			}
			else 
			{
				if(ground==BLUE)
					Speed_Y = SW_SPEED;
				if(ground==RED) 
					Speed_Y = -SW_SPEED;
				LCD_SetXY(0,7);
				LCD_Printf("Time out Cam_S!");		
				if(camera_data1.time_flag>100)
					buz_msg(1,2,3,200);
				else
					buz_msg(1,1,2,200);
			}
			break;
	}
	
	error_dis1 = Cam_Dist[Cam_Circle_Line] - Aim_D_Swing[Cam_Circle_Line];
	
	if(error_angle>pi)
		error_angle = error_angle - 2*pi;
	if(error_angle<-pi)
		error_angle = error_angle + 2*pi;
	
	if(!Cam_F_DISABLE)
	{
		if(camera_data1.time_flag<100)
		{	
			if(camera_data2.time_flag<100||camera_data3.time_flag<100)
				buz_msg(0,0,0,200);
			if(Abs(error_dis1)<2)
				Speed_X = 0;
			else 
				Speed_X = WT_D1_PID(error_dis1,Pid_List[21]);
					
			if(Speed_X>1000)
					Speed_X = 1000;
			if(Speed_X<-1000)
					Speed_X = -1000;
		}
		else
		{
				Speed_X = SW_SPEED;
				LCD_SetXY(0,6);
				LCD_Printf("Time out Cam_F!");
			  if(camera_data2.time_flag>100)
					buz_msg(1,2,3,200);
				else
			    buz_msg(1,1,1,200);
		}
	}
	
	if(Abs(error_angle)<0.01)
		Speed_Rotation = 0;
	else
    Speed_Rotation = 1000*AnglePID(error_angle,Pid_List[20]);
	
	if(Speed_Rotation>300)
		Speed_Rotation = 300;
	if(Speed_Rotation<-300)
		Speed_Rotation = -300;
	
	LCD_SetXY(0,0);
	LCD_Printf("c_d_f:%.1f e_d_f:%.1f",Cam_Dist[Cam_Circle_Line],error_dis1);
	LCD_SetXY(0,1);
	if(REDFlag||GROUND==RED)
		LCD_Printf("c_d_s:%.1f e_d_s:%.1f",Cam_Dist[Cam_Line_RED],error_dis2);
	else
		LCD_Printf("c_d_s:%.1f e_d_s:%.1f",Cam_Dist[Cam_Line_BLUE],error_dis2);
	LCD_SetXY(0,2);
	LCD_Printf("A:%.2f    e_a:%.2f",Gyro.angle,error_angle/pi*180.0);	
	LCD_SetXY(0,3);
	LCD_Printf("Vout_X:%.2f",Speed_X);
	LCD_SetXY(0,4);
	LCD_Printf("Vout_Y:%.2f",Speed_Y);	
	LCD_SetXY(0,5);
	LCD_Printf("Vout_A:%.2f",Speed_Rotation);	
	
	SD_Printf_MQ(&cam_file,"%.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\r\n",Cam_Dist[Cam_Circle_Line],Cam_Dist[Cam_Line_BLUE],Cam_Dist[Cam_Line_RED],Gyro.angle,Speed_X,Speed_Y,Speed_Rotation,GPS.position.x,GPS.position.y);	
	
 	
}

void SetCamCircle(fp64 aim_a,fp64 aim_d,fp64 aim_a_g,u8 place)
{
	Aim_A_Circle[place] = aim_a;
	Aim_D_Circle[place] = aim_d;
	Aim_A_G_Circle[place] = aim_a_g;
}

void SetGoCamCircle(fp64 aim_a,fp64 aim_d)
{
	Aim_Angle = aim_a;
	Aim_Dist  = aim_d;
}


void Keep_Circle(u8 place)
{
  fp64 error_dis,e_gyro_angle,e_cam_angle;
	static u8 flag;
	
	error_dis = Cam_Dist[Cam_Circle_Line] - Aim_D_Circle[place];
	e_gyro_angle = GPS.radian - Aim_A_G_Circle[place]/180.0*pi;
	e_cam_angle = Cam_Angle[Cam_Circle_Line] - Aim_A_Circle[place];
	
	if(e_gyro_angle>pi)
		e_gyro_angle = e_gyro_angle - 2*pi;
	if(e_gyro_angle<-pi)
		e_gyro_angle = e_gyro_angle + 2*pi;	
	
	if(e_cam_angle>180)
		e_cam_angle = e_cam_angle - 360;
	if(e_cam_angle<-180)
		e_cam_angle = e_cam_angle + 360;
	
	if(place>=4)
	{
		SPEED_STOP;
		LCD_Clear();
		LCD_SetXY(0,1);
		LCD_Printf("place error!");
	}
	else
	{
		if(camera_data1.time_flag<100)
		{	
//			flag = 0;
			if(Abs(e_gyro_angle)<0.01)
				Speed_Rotation = 0;
			else 
				Speed_Rotation = 1000*AnglePID(e_gyro_angle,Pid_List[20]);
			
			if(Speed_Rotation>300)
				Speed_Rotation = 300;
			if(Speed_Rotation<-300)
				Speed_Rotation = -300;
			
			if(Abs(error_dis)<stop_ed_static||Abs(e_gyro_angle)>0.1)
				Speed_X = 0;
			else 
				Speed_X = DistancePID(error_dis,Pid_List[12]);
			
			if(Abs(e_cam_angle)<stop_ea_static||Abs(e_gyro_angle)>0.1)
				Speed_Y = 0;
			else 
				Speed_Y = Cam_A_PID(e_cam_angle,Pid_List[13]);				
		 } 
		
		else 
		{
//			if(!flag)
//			{
//				flag = 1;
//			  SPEED_STOP;
//			}
			SPEED_STOP;
			LCD_Clear();
			LCD_SetXY(0,1);
			LCD_Printf("Long time data not received!");
		}
		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("c_a:%.1f e:%.2f",Cam_Angle[Cam_Circle_Line],e_cam_angle);
		LCD_SetXY(0,1);
		LCD_Printf("g_a:%.1f e:%.2f",Gyro.angle,e_gyro_angle/pi*180.0);
		LCD_SetXY(0,2);
		LCD_Printf("c_d:%.1f e:%.2f",Cam_Dist[Cam_Circle_Line],error_dis);
		LCD_SetXY(0,3);
		LCD_Printf("Vout_A:%.2f",Speed_Rotation);
		LCD_SetXY(0,4);
		LCD_Printf("Vout_X:%.2f",Speed_X);
		LCD_SetXY(0,5);
		LCD_Printf("Vout_Y:%.2f",Speed_Y);	
	}
}

void Keep_Angle(u8 place)
{
	fp64 error_angle;

	switch(place)
	{
		case Angle_Swing_BLUE:
			error_angle = GPS.radian - Aim_A_Swing[BLUE]/180.0*pi;
			break;
		case Angle_Swing_RED:
			error_angle = GPS.radian - Aim_A_Swing[RED]/180.0*pi;
			break;
		case Angle_Pole:
			error_angle = GPS.radian - Aim_A_Pole/180.0*pi;
			break;
		case Angle_Circle_Jg:
			error_angle = GPS.radian - Aim_A_G_Circle[Circle_jg]/180.0*pi;
	}

	if(error_angle<-pi)
		error_angle += 2*pi;
	if(error_angle>pi)
		error_angle -= 2*pi;
	
	if(Abs(error_angle)<0.02)
		Speed_Rotation = 0;
	else
		Speed_Rotation = 1000*AnglePID(error_angle,Pid_List[20]);

	if(Speed_Rotation>300)
		Speed_Rotation = 300;
		
	if(Speed_Rotation<-300)
		Speed_Rotation = -300;
	
	LCD_Clear();
	LCD_SetXY(0,2);
	LCD_Printf("A:%.2f    e_a:%.2f",Gyro.angle,error_angle/pi*180.0);
	LCD_SetXY(0,5);
	LCD_Printf("Vout_A:%.2f",Speed_Rotation);
}


/**********************************END OF FILE*********************************/
 
