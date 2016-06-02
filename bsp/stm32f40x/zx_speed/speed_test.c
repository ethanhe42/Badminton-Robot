#include "includes.h"
#include "math.h"
#define SQR 1.41421356
#include "speed_test.h"
#include "mecanum.h"
static	rt_uint32_t key_value;
int speed_x_t(void);
int speed_y_t(void);
int speed_xy_t(void);
void Spe_Acc_get(double *speed_really);
void acc_set(void);
//void delay_set(void);
uint16_t motor_really(char *str);

static double acc_vaule_x;
static double acc_vaule_y;
static int delay_time;


FIL t_dis_y;
FIL t_dis_x;
FIL t_dis_xy;
FIL t_spe_y;
FIL t_spe_x;
FIL t_spe_xy;
FIL position_x;
FIL position_y;
FIL position_xy;
FIL t_spe;
/************************测x轴方向的最大速度***************************/

void test_speed_x(void)
{
	static int gotime=0;

    int flag=1;
//    double speed_really;
		char string;
    acc_set();

    f_open (&t_spe_x, "test_speed_x.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
    f_open (&position_x, "position_x.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
		f_open (&t_spe, "position_x.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

    while(flag)
    {
        Delay_ms(20);
        gotime+=5;  
				
//				GoLine();

				Speed_X=realspe+acc_vaule_x;
				SetSpeed(Speed_X,0,0);
				
//        Spe_Acc_get(&speed_really);
				motor_really(&string);
        SD_Printf_MQ(&t_spe_x,"%d     %f     %d     %f\r\n",gotime,realspe,gotime,Speed_X);
        SD_Printf_MQ(&position_x,"%f      %f       %d\r\n",GPS.position.x,GPS.position.y,gotime);
				SD_Printf_MQ(&t_spe,"%f      %f    \r\n",realspe_x,realspe_y);

        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            switch(key_value)
            {
            case key4:
                flag=0;
                break;
            }
        }


        LCD_Clear();
        LCD_SetXY(0,0);
        LCD_WriteString("1.A:");
        LCD_WriteDouble(GPS.radian);

        LCD_SetXY(0,1);
        LCD_WriteString("2.X:");
        LCD_WriteDouble(GPS.position.x);

        LCD_SetXY(0,2);
        LCD_WriteString("3.Y:");
        LCD_WriteDouble(GPS.position.y);

        LCD_SetXY(0,3);
        LCD_WriteString("4.STOP");

        LCD_SetXY(0,4);
        LCD_WriteString("5.speed_set:");
        LCD_WriteDouble(Speed_X);

        LCD_SetXY(0,5);
        LCD_WriteString("6.speed_really:");
        LCD_WriteDouble(realspe);

    }
    SD_Printf_MQ(&t_spe_x,"THE END");
    SD_Printf_MQ(&position_x,"THE END");
    SD_Printf_MQ(&t_spe,"THE END");

}

/************************测y轴方向的最大速度***************************/
void test_speed_y(void)
{
    int flag=1;		char string;
//    double speed_really;
static int gotime=0;

    acc_set();

    f_open (&t_spe_y, "test_speed_y.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
    f_open (&position_y, "position_y.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
    f_open (&t_spe, "t_spe.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
    while(flag)
    {
				Delay_ms(5);
        gotime+=5;
				

				Speed_Y=realspe+acc_vaule_y;
				SetSpeed(0,Speed_Y,0);
				motor_really(&string);
			
        SD_Printf_MQ(&t_spe_y,"%d    %f    %d  %f\r\n",gotime,realspe,gotime,Speed_Y);
         SD_Printf_MQ(&t_spe,"%f           %f\r\n",realspe_x,realspe_y);
        SD_Printf_MQ(&position_y,"%d    %d     %f\r\n",GPS.position.x,GPS.position.y,gotime);


        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            switch(key_value)
            {
            case key4:
                flag=0;
                break;
            }
        }


        LCD_Clear();
        LCD_SetXY(0,0);
        LCD_WriteString("1.A:");
        LCD_WriteDouble(GPS.radian);

        LCD_SetXY(0,1);
        LCD_WriteString("2.X:");
        LCD_WriteDouble(GPS.position.x);

        LCD_SetXY(0,2);
        LCD_WriteString("3.Y:");
        LCD_WriteDouble(GPS.position.y);

        LCD_SetXY(0,3);
        LCD_WriteString("4.STOP");

        LCD_SetXY(0,4);
        LCD_WriteString("5.speed_set:");
        LCD_WriteDouble(Speed_Y);

        LCD_SetXY(0,5);
        LCD_WriteString("6.speed_really:");
        LCD_WriteDouble(realspe);

    }
    SD_Printf_MQ(&t_spe_y,"THE END");
    SD_Printf_MQ(&position_y,"THE END");
    SD_Printf_MQ(&t_spe,"THE END");
}

/***************************斜测45度**********************************/
void test_speed_xy(void)
{
    static int flag_speed=1;
		static int gotime=0;

		acc_set();
//		delay_set();

    f_open (&t_spe_xy, "test_speed_xy.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

    while(flag_speed)
    {
			Speed_X=acc_vaule_x;
      Speed_Y=acc_vaule_y;
			
			SetSpeed(Speed_X,Speed_Y,0);
      Delay_ms(5);
      gotime+=5;
			
      SD_Printf_MQ(&t_spe_xy,"%d    %f    %f   \r\n",gotime,Speed_X,Speed_Y);
//				
//			if(gotime>=delay_time)
//			{		
//				flag_speed=0;
//				while(1)
//					SetSpeed(0,0,0);
//				break;
//			}

			if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
					switch(key_value)
            {
            case key4:
							flag_speed=0;
						break;
            }
        }
		

        LCD_Clear();
        LCD_SetXY(0,0);
        LCD_WriteString("1.A:");
        LCD_WriteDouble(GPS.radian);

        LCD_SetXY(0,1);
        LCD_WriteString("2.Speed_X:");
        LCD_WriteDouble(Speed_X);

        LCD_SetXY(0,2);
        LCD_WriteString("3.Speed_Y:");
        LCD_WriteDouble(Speed_Y);

        LCD_SetXY(0,3);
        LCD_WriteString("4.STOP");

        LCD_SetXY(0,4);
        LCD_WriteString("5.speed_set:");
        LCD_WriteDouble(acc_vaule_x==0? acc_vaule_y:acc_vaule_x);

        LCD_SetXY(0,5);
        LCD_WriteString("6.speed_really:");
        LCD_WriteDouble(realspe);

    }
		SetSpeed(0,0,0); 
    SD_Printf_MQ(&t_spe_xy,"THE END");
//    SD_Printf_MQ(&position_xy,"THE END");
//		SD_Printf_MQ(&t_spe,"THE END");
}

void acc_set(void)
{
    static struct Input_ListValue Values[]=
    {
        {&acc_vaule_x,	TYPE_FP64,	"acc_x"},
        {&acc_vaule_y,	TYPE_FP64,	"acc_y"},
    };

    LCD_Clear();
    GPS_Clear();
    PID_Clear();

    Input_List(Values,2);

    if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);

}
void Spe_Acc_get(double *speed_really)
{
		double yiqi[5][2]={0};
		double speed[5]={0};
		int n;
		n=n%5;			
			//获取坐标
		yiqi[n][0]=MISO_GPS_X.f64_data;
		yiqi[n][1]=MISO_GPS_Y.f64_data;  
			
		if(n!=0)
		{
			speed[n]=sqrt((yiqi[n-1][0]-yiqi[n][0])*(yiqi[n-1][0]-yiqi[n][0])+(yiqi[n][1]-yiqi[n-1][1])*(yiqi[n][1]-yiqi[n-1][1]))/0.02;
		}
		else
			speed[0]=sqrt((yiqi[4][0]-yiqi[0][0])*(yiqi[4][0]-yiqi[0][0])+(yiqi[4][1]-yiqi[0][1])*(yiqi[4][1]-yiqi[0][1]))/0.02;
			
			//通过坐标算得实际速度
			
		if(n==0)
		{
			*speed_really=(speed[4]+speed[3]+speed[2]+speed[0])/4;
		}
		else if(n==1)
		{
			*speed_really=(speed[1]+speed[0]+speed[4]+speed[3])/4;
		}
		else if(n==2)
		{
			*speed_really=(speed[2]+speed[1]+speed[0]+speed[4])/4;
		}
		else if(n==3)
		{
			*speed_really=(speed[3]+speed[2]+speed[1]+speed[0])/4;
		}		
		else if(n==4)
		{
			*speed_really=(speed[4]+speed[3]+speed[2]+speed[1])/4;
		}	
		n++;
}
uint16_t motor_really(char * str)
{
	uint16_t dis;
	/*
	Com_SendByte(4,0x70);
	Com_SendByte(4,0x6f);
	Com_SendByte(4,0x75);
	Com_SendByte(4,0x20);
	*/
	Com_Puts(4,str);
	return dis;
}


//void delay_set(void)
//{
//    static struct Input_ListValue Values[]=
//    {
//        {&delay_time,	TYPE_INT32,	"delay"},
//    };

//    LCD_Clear();
//    GPS_Clear();
//    PID_Clear();

//    Input_List(Values,1);

//    if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
//}
// 