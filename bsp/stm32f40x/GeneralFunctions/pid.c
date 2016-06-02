
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

u8 PID_flag=1;

struct PID Pid_List[]=
{
	{0,1000000,0},	//0  跑圆角度PID
	{0,1000000,0},	//1  跑圆距离PID
	
	{0.8,1000000,0},	//2  直线角度PID
	{9,1000000,8},	//3  直线距离PID
	
	{0.6,1000000,0},	//4  Keep角度PID
	{6,1000000,0},		//5  Keep距离PID
	
	{1.1,1000000,3},//6  跑点角度PID
	{8,1000000,0},	//7  跑点距离PID  
	
	{0,1000000,0},	//8  灯板角度PID
	{0,1000000,0},  //9  灯板距离PID
	{0,1000000,0},  //10  
	{0,1000000,0},  //11 
	
	{60,5000,1},  //12  摄像头靠圆定位距离PID   
	{-130,5000,1},	//13  摄像头靠圆定位角度PID
	
	{10,1000,0},	//14  交接摄像头前后电机PID
	{1.5,600,0},	//15  交接摄像头底盘PID
	{20,10000,0},		//16  交接摄像头zPID

	{35,500,0},//17  交接摄像头角度PID
	
	{-10,3000,0},  	//18  看盘子摄像头xPID
	{-50,10000,1},	//19  看盘子摄像头yPID
	{-0.5,10000,1},	//20  看盘子摄像头角度PID
	
	{30,3000,0},	//21  白线前面摄像头距离PID
	{25,3000,0},	//22  白线旁边摄像头距离PID
	
	{10,100000,0.1},  //23  秋千处交接摄像头前后电机PID
	{-0.5,100000,0.1},  //24  秋千处交接摄像头底盘PID
	{20,100000,0.1}, //25  秋千处交接摄像头zPID
	
	{-20,10000,3},  //26  摄像头跑圆定位距离PID    by：ZhangHui 4.15 
	{0.5,5000,3},	//27  摄像头跑圆定位角度PID		by：ZhangHui 4.15 
	{1,1000000,0}, //28 手柄调正PID  杨坚
{0,1000000,0}, //29 ducted fan
};
																								
static double A_ErrorPast1=0;
static double A_ErrorPast2=0;
static double A_Mend=0;
static double D_ErrorPast1=0;
static double D_ErrorPast2=0;
static double D_Mend=0;

fp64 Cam_X_EP1 = 0.0;
fp64 Cam_X_EP2 = 0.0;
fp64 Cam_Y_EP1 = 0.0;
fp64 Cam_Y_EP2 = 0.0;
fp64 Cam_Z_EP1 = 0.0;
fp64 Cam_Z_EP2 = 0.0;
fp64 Cam_A_EP1 = 0.0;
fp64 Cam_A_EP2 = 0.0;
fp64 WT_D1_EP1  = 0.0;
fp64 WT_D1_EP2  = 0.0;
fp64 WT_D2_EP1  = 0.0;
fp64 WT_D2_EP2  = 0.0;


fp64 Cam_X_AmendV = 0;
fp64 Cam_Y_AmendV = 0;
fp64 Cam_Z_AmendV = 0; 
fp64 Cam_A_AmendV = 0;
fp64 WT_D1_AmendV  = 0;
fp64 WT_D2_AmendV  = 0;




fp64 Vout[5]={0.0};


/*
 * 函数名: PID_Clear
 * 描  述: 将PID存储的误差值清除
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void PID_Clear()
{
	int i;
	 
	for(i=0;i<28;i++)
	{
		Pid_List[i].mend=0;
		Pid_List[i].ep1=0;
		Pid_List[i].ep2=0;
	}
	
	A_ErrorPast1 = 0.0;
	A_ErrorPast2 = 0.0;
	A_Mend = 0.0;
	D_ErrorPast1 = 0.0;
	D_ErrorPast2 = 0.0;
	D_Mend = 0.0;
	
	Cam_X_EP1 = 0.0;
	Cam_X_EP2 = 0.0;
	Cam_Y_EP1 = 0.0;
	Cam_Y_EP2 = 0.0;
	Cam_Z_EP1 = 0.0;
	Cam_Z_EP2 = 0.0;
	Cam_A_EP1 = 0.0;
	Cam_A_EP2 = 0.0;
	WT_D1_EP1  = 0.0;
	WT_D1_EP2  = 0.0;
	WT_D2_EP1  = 0.0;
	WT_D2_EP2  = 0.0;


	Cam_X_AmendV = 0.0;
	Cam_Y_AmendV = 0.0;
	Cam_Z_AmendV = 0.0;
	Cam_A_AmendV = 0.0;
	WT_D1_AmendV  = 0.0;
	WT_D2_AmendV  = 0.0;
	
	for(i=0;i<5;i++)
	   Vout[i]=0;
}

/*
 * 函数名: PID
 * 描  述: 位置式PID调整，利用了增量式的递推
 * 输  入: -Error 误差量
 *		   -pid pid参数
 * 输  出: 反馈结果
 * 调  用: 外部调用
 */
double PID(double Error,struct PID pid)
{
	double A=0;
	double B=0;
	double C=0;
	double delta=0;
	
	A = (pid.p*(1+1/pid.i+pid.d));
	B = -pid.p*(1+2*pid.d);
	C = pid.p*pid.d;
	
	delta = A*Error + B*pid.ep1 + C*pid.ep2;
	pid.mend += delta;
	
	pid.ep2 = pid.ep1;
	pid.ep1 = Error;
	
	return pid.mend;
}


/*
 * 函数名: AnglePID
 * 描  述: 位置式角度PID调整，利用了增量式的递推
 * 输  入: -Error 误差量
 *		   -pid pid参数
 * 输  出: 角速度
 * 调  用: 外部调用
 */
double AnglePID(double Error,struct PID pid)
{
	double A=0;
	double B=0;
	double C=0;
	double delta=0;

	
	A = (pid.p*(1+1/pid.i+pid.d));
	B = -pid.p*(1+2*pid.d);
	C = pid.p*pid.d;
	
	delta = A*Error + B*A_ErrorPast1 + C*A_ErrorPast2;
	A_Mend += delta;
	
	A_ErrorPast2=A_ErrorPast1;
	A_ErrorPast1=Error;
	
	return A_Mend;
}

/*
 * 函数名: DistancePID
 * 描  述: 位置式距离PID调整，利用了增量式的递推
 * 输  入: -Error 误差量
 *		   -pid pid参数
 * 输  出: 速度
 * 调  用: 外部调用
 */
double DistancePID(double Error,struct PID pid)
{
	double A=0;
	double B=0;
	double C=0;
	double delta=0;
	
	A = (pid.p*(1+1/pid.i+pid.d));
	B = -pid.p*(1+2*pid.d);
	C = pid.p*pid.d;
	
	delta = A*Error + B*D_ErrorPast1 + C*D_ErrorPast2;
	D_Mend+=delta;
	
	D_ErrorPast2=D_ErrorPast1;
	D_ErrorPast1=Error;
	
	return D_Mend;
}

fp64 Cam_X_PID(s16 error_dis,struct PID pid)
{
	fp64  Cam_a0 = 0.0;
	fp64  Cam_a1 = 0.0;
	fp64  Cam_a2 = 0.0;
	fp64  Cam_Vd=0.0;
	u8    Iflag=0;

	if(error_dis<20)//积分分离
		Iflag=1;

	Cam_a0 = ( pid.p * ( 1 + 1 / pid.i*Iflag + pid.d ));     
	Cam_a1 = ( -pid.p * (1 + 2 * pid.d));         
	Cam_a2 = ( pid.p * pid.d );    	
	Cam_Vd = Cam_a0 *error_dis + Cam_a1 *Cam_X_EP1 + Cam_a2 *Cam_X_EP2; 
	Cam_X_AmendV = Cam_X_AmendV + Cam_Vd;
	Cam_X_EP2 = Cam_X_EP1;
	Cam_X_EP1 = error_dis;

	return Cam_X_AmendV;
}

fp64 Cam_Y_PID(s16 error_dis,struct PID pid)
{
	fp64  Cam_a0 = 0.0;
	fp64  Cam_a1 = 0.0;
	fp64  Cam_a2 = 0.0;
	fp64  Cam_Vd=0.0;   
	u8    Iflag=0;

	if(error_dis<20)//积分分离
		Iflag=1;	

	Cam_a0 = ( pid.p * ( 1 + 1 / pid.i*Iflag + pid.d ));     
	Cam_a1 = ( -pid.p * (1 + 2 * pid.d));         
	Cam_a2 = ( pid.p * pid.d );    	
	Cam_Vd = Cam_a0 *error_dis + Cam_a1 *Cam_Y_EP1 + Cam_a2 *Cam_Y_EP2; 
	Cam_Y_AmendV = Cam_Y_AmendV + Cam_Vd;
	Cam_Y_EP2 = Cam_Y_EP1;
	Cam_Y_EP1 = error_dis;

	return Cam_Y_AmendV;
}

fp64 Cam_Z_PID(s16 error_dis,struct PID pid)
{
	fp64  Cam_a0 = 0.0;
	fp64  Cam_a1 = 0.0;
	fp64  Cam_a2 = 0.0;
	fp64  Cam_Vd=0.0;
	u8    Iflag=0;

	if(error_dis<50)//积分分离
		Iflag=1;

	Cam_a0 = ( pid.p * ( 1 + 1 / pid.i*Iflag + pid.d ));     
	Cam_a1 = ( -pid.p * (1 + 2 * pid.d));         
	Cam_a2 = ( pid.p * pid.d );   	
	Cam_Vd = Cam_a0 *error_dis + Cam_a1 *Cam_Z_EP1 + Cam_a2 *Cam_Z_EP2; 
	Cam_Z_AmendV = Cam_Z_AmendV + Cam_Vd;
	Cam_Z_EP2 = Cam_Z_EP1;
	Cam_Z_EP1 = error_dis;

	return Cam_Z_AmendV;
}

fp64 Cam_A_PID(double error_angle,struct PID pid)
{
	fp64  Cam_a0 = 0.0;
	fp64  Cam_a1 = 0.0;
	fp64  Cam_a2 = 0.0;
	fp64  Cam_Vd = 0.0;    


	Cam_a0 = ( pid.p * ( 1 + 1 / pid.i + pid.d ));     
	Cam_a1 = ( -pid.p * (1 + 2 * pid.d));         
	Cam_a2 = ( pid.p * pid.d );   	
	Cam_Vd = Cam_a0 *error_angle + Cam_a1 *Cam_A_EP1 + Cam_a2 *Cam_A_EP2; 
	Cam_A_AmendV = Cam_A_AmendV + Cam_Vd;
	Cam_A_EP2 = Cam_A_EP1;
	Cam_A_EP1 = error_angle;

	return Cam_A_AmendV;
}


fp64 WT_D1_PID(double error_dis,struct PID pid)
{
	fp64  WT_a0 = 0.0;
	fp64  WT_a1 = 0.0;
	fp64  WT_a2 = 0.0;
	fp64  WT_Vd = 0.0;    


	WT_a0 = ( pid.p * ( 1 + 1 / pid.i + pid.d ));     
	WT_a1 = ( -pid.p * (1 + 2 * pid.d));         
	WT_a2 = ( pid.p * pid.d );

	WT_Vd = WT_a0 *error_dis + WT_a1 *WT_D1_EP1 + WT_a2 *WT_D1_EP2; 
	WT_D1_AmendV = WT_D1_AmendV + WT_Vd;
	WT_D1_EP2 = WT_D1_EP1;
	WT_D1_EP1 = error_dis;

	return WT_D1_AmendV;
}

fp64 WT_D2_PID(double error_dis,struct PID pid)
{
	fp64  WT_a0 = 0.0;
	fp64  WT_a1 = 0.0;
	fp64  WT_a2 = 0.0;
	fp64  WT_Vd = 0.0;


	WT_a0 = ( pid.p * ( 1 + 1 / pid.i + pid.d ));
	WT_a1 = ( -pid.p * (1 + 2 * pid.d));
	WT_a2 = ( pid.p * pid.d );

	WT_Vd = WT_a0 *error_dis + WT_a1 *WT_D2_EP1 + WT_a2 *WT_D2_EP2;
	WT_D2_AmendV = WT_D2_AmendV + WT_Vd;
	WT_D2_EP2 = WT_D2_EP1;
	WT_D2_EP1 = error_dis;

	return WT_D2_AmendV;
}

/**********************************END OF FILE*********************************/
