
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"


/*
 * 函数名: EasyLine
 * 描  述: 简单版走直线功能
 * 输  入: -end_point  目标位置
 *		   -aim_radian 目标角度
 *		   -speed	   运行速度(绝对值无正负)
 * 输  出: 无
 * 调  用: 外部调用
 */
void EasyLine(struct Point end_point,double aim_radian,double speed)
{
	struct Point sta_point = GPS.position;		//记录初始位置

	double phi;									//
	
	double speed_x,speed_y;						//世界坐标里的速度与分速度
	double speed_robot_x,speed_robot_y;			//转换到机器人坐标的分速度
	
	double error_angle,error_dis;				//角度误差与距离误差
	double Vout_A=0;							//PID调整后角度输出值
	double Vout_D=0;							//PID调整后距离输出值
	double v_err_x,v_err_y;						//PID调整速度的分量
	
	double lineangle = GetLineAngle(sta_point,end_point);	//求出该路径线段的倾角
	
	PID_Clear();
	
	//求速度在x,y方向上的分量
	speed_x = speed*cos(lineangle);
	speed_y = speed*sin(lineangle);
	
	//角度误差和距离误差
	error_angle = aim_radian-GPS.radian;
	error_dis   = ( GPS.position.x - sta_point.x ) * sin( lineangle ) 
				- ( GPS.position.y - sta_point.y ) * cos( lineangle );
	
	if(error_angle>pi)
		error_angle-=2*pi;
	if(error_angle<-pi)
		error_angle+=2*pi;
	
	//PID调整(位置式)
	Vout_A = AnglePID(error_angle,Pid_List[2]);
	Vout_D = DistancePID(error_dis,Pid_List[3]);
	
	//PID调整速度的分量
	v_err_x = -Vout_D*sin(lineangle);
	v_err_y =  Vout_D*cos(lineangle);
	
	//加上调整增量
	speed_x += v_err_x;
	speed_y += v_err_y;
	
	//将世界坐标的速度转换为机器人局部坐标的速度
	phi = GPS.radian;
	speed_robot_x =  speed_x*cos(phi) + speed_y*sin(phi);
	speed_robot_y = -speed_x*sin(phi) + speed_y*cos(phi);
	
	//为四轮赋速度
	Speed_X = speed_robot_x;
	Speed_Y = speed_robot_y;
	Speed_Rotation = Vout_A;
}


/**********************************END OF FILE*********************************/
