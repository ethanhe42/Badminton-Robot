#include "includes.h"
#define PI 3.1415926

struct Pointfp64
{
	fp64 x;
	fp64 y;
};

struct Gps_State
{   
	struct Pointfp64  	position;	  	  //当前坐标	
	fp64				distancestep1; 	//码盘1本次前进距离
	fp64				distancestep2;  //码盘2本次前进距离
	fp64			  distance1;	  	//码盘1前进距离
	fp64			  distance2;		  //码盘2前进距离
	fp64  	    angle;        	//当前角度 		
	fp64				radian;			    //当前弧度
	fp64				speed;			    //当前速度
	fp64				acceleration;	  //当前加速度
	fp64			 	rotationspeed;	//当前自转速度
	fp64				rotationacc;	  //当前自转加速度
};
struct Encoder_Stat
{	
	int64_t Now;
	
	int64_t Last;
	
	int64_t Total;
	
	fp64 Convert1;		//正转系数
	
	fp64 Convert2;		//反转系数
	
	u8 dir;		//改变正反转方向（如果是需要的方向则置1，否则置-1）
	
	fp64 Distance;
	
	fp64  radian;
	
	fp64 Radius;
	
};

static struct Pointfp64 my_GPS;
static char thread_my_GPS_stack[1024];
struct rt_thread thread_my_GPS;


struct Encoder_Stat Encoders[2];
struct Gps_State Gps_List[2];
/*
 * 函数名: rt_thread_entry_my_GPS
 * 描  述: 算实际速度线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 */
static void rt_thread_entry_my_GPS(void* parameter)
{
	fp64 radian;
	fp64 radianstep = 0.0;						//弧度步进值
	fp64 localx;
	fp64 localy;
  
	while(1)
	{
		Delay_ms(5);
		Gps_List[1] = Gps_List[0];
	
		Gps_List[0].angle = MISO_GYRO_RAD.f64_data;//有原始值//角度范围为-180~180

		//转换为弧度值并限制在-PI~PI
		Gps_List[0].radian=Gps_List[0].angle*PI/180.0;

	
		//取上一时刻与现在时刻的角平分线做为该时间内车身的角度，前两个判断是为了解决在PI分界线处的突变问题
		radian=(Gps_List[0].radian+Gps_List[1].radian)/2;
		if(radian>PI)
			radian-=2*PI;
		if(radian<-PI)
			radian+=2*PI;
		
		Encoders[0].radian=MOSI_ENC1_RAD.f64_data;
		Encoders[1].radian=MOSI_ENC2_RAD.f64_data;
		Encoders[0].Radius=MOSI_ENC1_RAD.f64_data;
		Encoders[1].Radius=MOSI_ENC2_RAD.f64_data;
		
	//获得弧度步进值
		radianstep=Gps_List[0].radian-Gps_List[1].radian;
		if(radianstep>PI)
			radianstep-=2*PI;
		if(radianstep<-PI)
			radianstep+=2*PI;
		
		
		Encoders[0].Distance=MISO_ENC1_DIS.f64_data;
		Encoders[1].Distance=MISO_ENC2_DIS.f64_data;
	
			
		Gps_List[0].distance1 = Encoders[0].Distance;
		Gps_List[0].distance2 = Encoders[1].Distance;
		
		Gps_List[0].distancestep1 = Gps_List[0].distance1 - Gps_List[1].distance1 - Encoders[0].Radius*radianstep;
		Gps_List[0].distancestep2 = Gps_List[0].distance2 - Gps_List[1].distance2 - Encoders[1].Radius*radianstep; 
	
			
		localx=(-Gps_List[0].distancestep1*cos(Encoders[1].radian)+Gps_List[0].distancestep2*cos(Encoders[0].radian))/sin(Encoders[0].radian+Encoders[1].radian);
		localy=(Gps_List[0].distancestep1*sin(Encoders[1].radian)+Gps_List[0].distancestep2*sin(Encoders[0].radian))/sin(Encoders[0].radian+Encoders[1].radian);
		
		Gps_List[0].position.x+=localx*cos(radian)-localy*sin(radian);
		Gps_List[0].position.y+=localx*sin(radian)+localy*cos(radian);
		my_GPS.x=Gps_List[0].position.x;
		my_GPS.y=Gps_List[0].position.y;
	
	}	
	
}
 

/*
 * 函数名: rt_thread_my_GPS_init
 * 描  述: my_GPS刷新线程的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_my_GPS_init(void)
{
	rt_err_t result;

    
	result = rt_thread_init(&thread_my_GPS, 
                            "thread_my_GPS",
                            rt_thread_entry_my_GPS,
                            RT_NULL,
                            &thread_my_GPS_stack[0], sizeof(thread_my_GPS_stack),
							5, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_my_GPS);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
