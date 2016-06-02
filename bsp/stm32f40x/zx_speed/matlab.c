#include "includes.h"
#include "math.h"
#define PI 3.1415926

void fuck_stop(void);
static int flag=1;
static double acc_vaule_x,acc_vaule_y,acc_angle;
static	rt_uint32_t key_vaule;
void acc_set3(void);
void micromove(void)
{
	FIL micromove_original;
	FIL micromove_result;
	static int gotime=0;
	double speed_y=0,speed_x=0,angle=0;
	
	acc_set3();
	speed_y=acc_vaule_y;
	speed_x=acc_vaule_x;
	angle=acc_angle;
	
  f_open (&micromove_original, "micromove.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	f_open (&micromove_result, "micromove.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	
	while(flag)
	{
		Delay_ms(5);
		gotime+=5;
	
		if(gotime>=200)
			SetSpeed(speed_x,speed_y,angle);
		fuck_stop();
		SD_Printf_MQ(&micromove_original,"%d    %f    %f    %f\r\n",MISO_ENC1_DIS.f64_data,MISO_ENC2_DIS.f64_data);
		SD_Printf_MQ(&micromove_result,"%d     %f      %f      %f\r\n",gotime,MISO_GPS_X.f64_data,MISO_GPS_Y.f64_data,realspe);
	}
	SetSpeed(0,0,0);
	SD_Printf_MQ(&micromove_original,"THE END");
	SD_Printf_MQ(&micromove_result,"THE END");

}

void averagewalk(void)
{
	FIL averagewalk_original;
	FIL averagewalk_result;
	static int gotime=0;
	double speed_y=0,speed_x=0,angle=0;
	
	acc_set3();
	speed_y=acc_vaule_y;
	speed_x=acc_vaule_x;
	angle=acc_angle;

	f_open (&averagewalk_original, "averagewalk.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	f_open (&averagewalk_result, "averagewalk.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

	while(flag)
	{
		Delay_ms(5);
		gotime++;
	
		SetSpeed(speed_x,speed_y,angle);
		fuck_stop();
		SD_Printf_MQ(&averagewalk_original,"%d    %f    %f    %f\r\n",gotime,MISO_ENC1_ANG.f64_data,MISO_ENC1_DIS.f64_data,MISO_ENC2_DIS.f64_data);
		SD_Printf_MQ(&averagewalk_result,"%d     %f      %f      %f\r\n",gotime,MISO_GPS_X.f64_data,MISO_GPS_Y.f64_data,realspe);
	}
	SetSpeed(0,0,0);
	SD_Printf_MQ(&averagewalk_original,"THE END");
	SD_Printf_MQ(&averagewalk_result,"THE END");
}
//出现一走一停的问题 
void turnround(void)
{
	FIL turnround_original;
	FIL turnround_result;
	static int gotime=0;
	double speed_y=0,speed_x=0,angle=0;
	
	acc_set3();
	speed_y=acc_vaule_y;
	speed_x=acc_vaule_x;
	angle=acc_angle;
	
  f_open (&turnround_original, "turnround.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	f_open (&turnround_result, "turnround.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	
	while(flag)
	{
		Delay_ms(5);
		gotime++;
	
		SetSpeed(speed_x,speed_y,angle);
		fuck_stop();
		SD_Printf_MQ(&turnround_original,"%d    %f    %f    %f\r\n",gotime,MISO_ENC1_ANG.f64_data,MISO_ENC1_DIS.f64_data,MISO_ENC2_DIS.f64_data);
		SD_Printf_MQ(&turnround_result,"%d     %f      %f      %f\r\n",gotime,MISO_GPS_X.f64_data,MISO_GPS_Y.f64_data,realspe);
	}
	SetSpeed(0,0,0);
	SD_Printf_MQ(&turnround_original,"THE END");
	SD_Printf_MQ(&turnround_result,"THE END");

}

void anti_slip(void)
{
	FIL anti_slip_original;
	FIL anti_slip_result;
	static int gotime=0;
	double speed_y=0,speed_x=0,angle=0;
	
	acc_set3();

  f_open (&anti_slip_original, "anti_slip_original.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
  f_open (&anti_slip_result, "anti_slip_result.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

	while(flag)
	{
		Delay_ms(5);
		gotime++;
	
		speed_y=realspe+acc_vaule_y;
		SetSpeed(speed_x,speed_y,angle);
		fuck_stop();
		SD_Printf_MQ(&anti_slip_original,"%d    %f    %f    %f\r\n",gotime,MISO_ENC1_ANG.f64_data,MISO_ENC1_DIS.f64_data,MISO_ENC2_DIS.f64_data);
		SD_Printf_MQ(&anti_slip_result,"%d     %f      %f      %f\r\n",gotime,MISO_GPS_X.f64_data,MISO_GPS_Y.f64_data,realspe);
	}
	SetSpeed(0,0,0);
	SD_Printf_MQ(&anti_slip_original,"THE END");
	SD_Printf_MQ(&anti_slip_result,"THE END");
}
void slipwalk(void)
{
	FIL slipwalk_original;
	FIL slipwalk_result;
	static int gotime=0;
	double speed_y=0,speed_x=0,angle=0;
	
	acc_set3();

  f_open (&slipwalk_original, "slipwalk_original.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	f_open (&slipwalk_result, "slipwalk_result.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

	while(flag)
	{
		Delay_ms(5);
		gotime++;
	
		speed_y=realspe+acc_vaule_y;
		SetSpeed(speed_x,speed_y,angle);
		fuck_stop();
		SD_Printf_MQ(&slipwalk_original,"%d    %f    %f     %f\r\n",gotime,MISO_ENC1_ANG.f64_data,MISO_ENC1_DIS.f64_data,MISO_ENC2_DIS.f64_data);
		SD_Printf_MQ(&slipwalk_result,"%d     %f      %f      %f\r\n",gotime,MISO_GPS_X.f64_data,MISO_GPS_Y.f64_data,realspe);
	}
	SetSpeed(0,0,0);
	SD_Printf_MQ(&slipwalk_original,"THE END");
	SD_Printf_MQ(&slipwalk_result,"THE END");
}

void go_circle(void)
{
	FIL go_circle_original;
	FIL go_circle_result;
	static int gotime=0;
	double speed_y=0,speed_x=0,angle=0;
	Delay_ms(5);
	gotime++;
	
	acc_set3();

  f_open (&go_circle_original, "go_circle_original.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
	f_open (&go_circle_result, "go_circle_result.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);

	while(flag)
	{
		Delay_ms(5);
		gotime++;
	
		speed_y=realspe+acc_vaule_y;
		SetSpeed(speed_x,speed_y,angle);
		fuck_stop();
		SD_Printf_MQ(&go_circle_original,"%d    %f    %f     %f\r\n",gotime,MISO_ENC1_ANG.f64_data,MISO_ENC1_DIS.f64_data,MISO_ENC2_DIS.f64_data);
		SD_Printf_MQ(&go_circle_result,"%d     %f      %f      %f\r\n",gotime,MISO_GPS_X.f64_data,MISO_GPS_Y.f64_data,realspe);
	}
	SetSpeed(0,0,0);
	SD_Printf_MQ(&go_circle_original,"THE END");
	SD_Printf_MQ(&go_circle_result,"THE END");
}

void acc_set3(void)
{
    static struct Input_ListValue Values[]=
    {
        {&acc_vaule_x,	TYPE_FP64,	"acc_x"},
        {&acc_vaule_y,	TYPE_FP64,	"acc_y"},
				{&acc_angle,  	TYPE_FP64,	"acc_angle"},

    };

    LCD_Clear();
    GPS_Clear();
    PID_Clear();

    Input_List(Values,3);

    if(rt_mb_recv(&Mb_Key, &key_vaule, RT_WAITING_FOREVER) == RT_EOK);

}
void fuck_stop()
{
	if(rt_mb_recv(&Mb_Key, &key_vaule, RT_WAITING_NO) == RT_EOK)
        {
					switch(key_vaule)
            {
            case key4:
							flag=0;
						break;
            }
        }
		
        LCD_Clear();
        LCD_SetXY(0,0);
        LCD_WriteString("1.stop:");
}
