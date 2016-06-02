/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern int L_CH,B_CH,F_CH,R_CH;
extern int fantest;
extern int Swe_Acc;
extern int Swe_Acc_d;
extern double RATE;

int single=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*
 * 函数名: sys_Gyro
 * 描  述: 陀螺仪相关功能
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void RotatePID(void)
{
	rt_uint32_t key_value;
	double error_angle_this = 0;
	double Vout_aa = 0;
	struct PID pid_Keep;
	pid_Keep.i = 1000000;
	pid_Keep.p = -4;
	pid_Keep.d = 0;
 	Input_DoubleValue(&pid_Keep.p,"P");
	Input_DoubleValue(&pid_Keep.i,"I");
	Input_DoubleValue(&pid_Keep.d,"D");
	while(1)
	{
		error_angle_this = (0 - GPS.radian);
		if(error_angle_this>pi)
			error_angle_this-=2*pi;
		if(error_angle_this<-pi)
			error_angle_this+=2*pi;
		if(Abs(error_angle_this)<0.001)
			Speed_Rotation = 0;
		else
		{
			Vout_aa = 100 * AnglePID(error_angle_this,pid_Keep);
			Speed_Rotation = Vout_aa;
		}
		SetSpeed(Speed_X + Handle_Speed_X,Speed_Y + Handle_Speed_Y,-Speed_Rotation);
		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("P:");
		LCD_WriteDouble(pid_Keep.p);
		LCD_SetXY(0,1);
		LCD_WriteString("I:");
		LCD_WriteDouble(pid_Keep.i);
		LCD_SetXY(0,2);
		LCD_WriteString("D:");
		LCD_WriteDouble(pid_Keep.d);
		LCD_SetXY(0,3);
		LCD_WriteString("GPS_Angle:");
		LCD_WriteDouble(Gyro.angle);
		LCD_SetXY(0,4);
		LCD_WriteString("Vout_a:");
		LCD_WriteDouble(Vout_aa);	
		LCD_SetXY(0,5);
		LCD_WriteString("1.CLear GPS Angle:");
		LCD_SetXY(0,6);
		LCD_WriteString("6.single");
			
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				
				case key1:
					GPS_Clear();
					break;
				case key6:
	Input_IntValue(&single,"single");
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
	
	
	
//		pid_Keep.p = 300   ;
//		pid_Keep.i = 1000000;
//		pid_Keep.d = 0;

}
 
void sys_Gyro()
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("========Gyro========");
		LCD_SetXY(0,1);
		LCD_WriteString("angle:");
		LCD_WriteDouble(Gyro.angle);
		LCD_SetXY(0,2);
		LCD_WriteString("Con1:");
		LCD_WriteDouble(Gyro.convert1);
		LCD_SetXY(0,3);
		LCD_WriteString("Con2:");
		LCD_WriteDouble(Gyro.convert2);
		LCD_SetXY(0,4);
		LCD_WriteString("1.Init 2.Set 3.Float");  
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Gyro_Init();
					break;
				case key2:
					Gyro_Set();
					break;
				case key3:
					Gyro_CheckFloat();
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: sys_Encoder
 * 描  述: 码盘相关功能
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void sys_Encoder()
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Encoder=======");
		LCD_SetXY(0,1);
		LCD_WriteString("Dis1:");
		LCD_WriteDouble(Encoder[0].dis);
		LCD_SetXY(0,2);
		LCD_WriteString("Dis2:");
		LCD_WriteDouble(Encoder[1].dis);
		LCD_SetXY(0,3);
		LCD_WriteString("1.Init_Con 2.Init_R");
		LCD_SetXY(0,4);
		LCD_WriteString("3.View 4.Set");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Encoder_Init_Con();
					break;
				case key2:
					Encoder_Init_R();
					break;
				case key3:
					Encoder_View();
					break;
				case key4:
					Encoder_Set();
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: sys_GPS
 * 描  述: GPS显示与修改
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */

extern double StartAngle;
void sys_GPS()
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("========GPS=========");
		LCD_SetXY(0,1);
		LCD_WriteString("Angle:");
		LCD_WriteDouble(GPS.radian*180/pi);
		LCD_SetXY(0,2);
		LCD_WriteString("X:");
		LCD_WriteDouble(GPS.position.x);
		LCD_SetXY(0,3);
		LCD_WriteString("Y:");
		LCD_WriteDouble(GPS.position.y);
		LCD_SetXY(0,4);
		LCD_WriteString("1.SetA 2.SetX 3.SetY");
		LCD_SetXY(0,5);
		LCD_WriteString("4.Clear");
		LCD_SetXY(0,6);
		LCD_WriteString("5.Record 6.Set");
		LCD_SetXY(0,7);
		LCD_Printf("StartA:%f",StartAngle);
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Input_DoubleValue(&MOSI_GPS_ANG.f64_data,"angle");
					MOSI_GPS_ANG.f64_data=MOSI_GPS_ANG.f64_data*pi/180;
					Write_Database(W_GPS_ANG_ID);
					break;
				case key2:
					Input_DoubleValue(&MOSI_GPS_X.f64_data,"X");
					Write_Database(W_GPS_X_ID);
					break;
				case key3:
					Input_DoubleValue(&MOSI_GPS_Y.f64_data,"Y");
					Write_Database(W_GPS_Y_ID);
					break;
				case key4:
					GPS_Clear();
					break;
				case key5:
					StartAngle = GPS.radian*180/pi;
					break;
				case key6:
					MOSI_GPS_ANG.f64_data=StartAngle*pi/180;
					Write_Database(W_GPS_ANG_ID);
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(20);
	}
}

/*
 * 函数名: sys_Mecanum
 * 描  述: 全向轮相关功能
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void sys_Swedish()
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Swedish=======");
		LCD_SetXY(0,1);
		LCD_WriteString("1.FL port:");
		LCD_WriteInt(Swe_FL.port);
		LCD_SetXY(0,2);
		LCD_WriteString("2.FR port:");
		LCD_WriteInt(Swe_FR.port);
		LCD_SetXY(0,3);
		LCD_WriteString("3.BL port:");
		LCD_WriteInt(Swe_BL.port);
		LCD_SetXY(0,4);
		LCD_WriteString("4.BR port:");
		LCD_WriteInt(Swe_BR.port);
		LCD_SetXY(0,5);
// 		LCD_WriteString("5.u and d:");
// 		LCD_WriteDouble(Swe_Acc);
// 		LCD_WriteDouble(Swe_Acc_d);
		LCD_WriteString("5.test_speed_xy");
		LCD_SetXY(0,6);
		LCD_WriteString("7.Change RATE:");		
// 		LCD_SetXY(0,6);
// 		LCD_WriteString("7.Change Acc:");
// 		LCD_SetXY(0,7);
// 		LCD_WriteString("8.Change Acc_d:");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Swedish_test(&Swe_FL);
					break;
				case key2:
					Swedish_test(&Swe_FR);
					break;
				case key3:
					Swedish_test(&Swe_BL);
					break;
				case key4:
					Swedish_test(&Swe_BR);
					break;
				case key6:
					test_speed_xy();
				case key7:
					Input_DoubleValue(&RATE,"RATE");
					break;
// 				case key7:
// 					Input_IntValue(&Swe_Acc,"Acc");
// 					break;
// 				case key8:
// 					Input_IntValue(&Swe_Acc_d,"Acc_d");
// 					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: sys_SA_ST
 * 描  述: 接近开关、接触开关
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void sys_SA_ST()
{
	rt_uint32_t key_value;
	uint8_t st_num=8;
	uint8_t sa_num=5;
	int i,j;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======SA-ST========");
		
		for(i=1;i<=st_num;i++)
		{
			LCD_SetXY(((i-1)%3)*7,(i-1)/3+1);
			LCD_Printf("ST%d:%d",i,ST_READ(i) );
		}
		
		j=(i-1)/3+2;
		
		for(i=1;i<=sa_num;i++)
		{
			LCD_SetXY(((i-1)%3)*7,j+(i-1)/3+1);
			LCD_Printf("SA%d:%d",i,SA_READ(i) );
		}
		
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
 * 函数名: sys_CAN
 * 描  述: CAN总线
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void sys_CAN()
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("========CAN=========");
		LCD_SetXY(0,1);
		LCD_WriteString("1.Relay");
		LCD_SetXY(0,2);
		LCD_WriteString("2.Motor");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Relay_View();
					break;
				case key2:
					Motor_View();
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: sys_Handle
 * 描  述: 手柄数据显示
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void sys_Handle()
{
	rt_uint32_t key_value;
	int i;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======Handle=======");
		LCD_SetXY(0,1);
		LCD_WriteString("UD:");
		LCD_WriteInt(HandleData_Y.lr-12288);
		LCD_SetXY(0,2);
		LCD_WriteString("LR:");
		LCD_WriteInt(HandleData_X.lr-12288);
		LCD_SetXY(0,3);	
		LCD_WriteString("Button_X:");
		for(i=0;i<3;i++)
		{
			LCD_Printf("%X",HandleData_X.rest[i]);
			LCD_WriteString("  ");
		}
		LCD_SetXY(0,4);
		LCD_WriteString("Button_Y:");
		for(i=0;i<3;i++)
		{
			LCD_Printf("%X",HandleData_Y.rest[i]);
			LCD_WriteString("  ");
		}
		LCD_SetXY(0,5);
		LCD_WriteInt(*handle_count);
		LCD_SetXY(0,6);
		LCD_WriteInt(*handle_count_y);		
		
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


#include "fan.h"
void sys_Fan(void)
{
	rt_uint32_t key_value;
    static double delta = 1.0;
	Fan_Set_Min();//???????
	fantest=1;
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("========Fan=========");
		LCD_SetXY(0,1);
		LCD_Printf("1:fan_up");
		LCD_SetXY(10, 1);
		LCD_Printf("2:fan_dn");
		LCD_SetXY(0,2);
		LCD_Printf("3:fan_max");
		LCD_SetXY(10,2);
		LCD_Printf("4:fan_min");

		LCD_SetXY(0,3);
		LCD_Printf("5.testend");
		
		LCD_SetXY(0,4);
		LCD_Printf("6.F:%f",get_fan_duty(F_CH));
		LCD_SetXY(0,5);
		LCD_Printf("7.R:%f",get_fan_duty(R_CH));
		LCD_SetXY(0,6);
		LCD_Printf("8.L:%f",get_fan_duty(L_CH));
		LCD_SetXY(0,7);
		LCD_Printf("9.B:%f",get_fan_duty(B_CH));
		
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
                    Fan_Duty_Up(delta);
					break;
				case key2:
					Fan_Duty_Dn(delta);
					break;
				case key3:
                    Fan_Set_Max();
					break;
				case key4:
					Fan_Set_Min();
				break;
				case key5:
					 fantest=0;
					break;
				case key6:
					Ductedfan_test(&F_CH);
					break;
				case key7:
					Ductedfan_test(&R_CH);
					break;
				case key8:
					Ductedfan_test(&L_CH);
					break;
				case key9:
					Ductedfan_test(&B_CH);
					break;
				case keyback:
					
					return;
				default:
					break;
			}
		}
		Delay_ms(10);
	}
}

/********************************    END OF FILE    ***************************/
