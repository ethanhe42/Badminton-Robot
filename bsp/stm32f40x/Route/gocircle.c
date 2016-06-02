/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"

struct Point Center;
double R;
double Speed;

/*
 * 函数名: SetCircle
 * 描  述: 初始化跑圆功能
 * 输  入: -center  圆心位置
 *		   -r 		圆的半径
 *		   -speed  	速度
 * 输  出: 无
 * 调  用: 外部调用
 */
void SetCircle(struct Point center,double r,double speed)
{
	Center = center;
	R = r;
	Speed = speed;
}

void GoCircle(void)
{
	//struct Point pos;
	double radian;
	double error_dis,error_angle;
	double Vout_D,Vout_A;
	
	radian = GetLineAngle(GPS.position,Center);
	
	//得到理论坐标与理论角度
	//pos.x = Center.x + R*cos(radian);
	//pos.y = Center.y + R*sin(radian);
	
	//角度误差和距离误差
	error_dis = GetLength(GPS.position,Center) - R;
	error_angle = radian - GPS.radian;
	
	if(error_angle>pi)
		error_angle-=2*pi;
	if(error_angle<-pi)
		error_angle+=2*pi;
	
	//PID调整
	Vout_D = DistancePID(error_dis,Pid_List[1]);
	Vout_A = 1000*AnglePID(error_angle,Pid_List[0]);
	
	Speed_Y = Speed;
	Speed_X = Vout_D;
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
	LCD_WriteDouble(radian);
	
}

 /**********************************END OF FILE*********************************/
