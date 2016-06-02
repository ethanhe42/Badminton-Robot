

#include "includes.h"
#include "sonic1.h"
#include "fxh_bool.h"



/**-------------------------------------------------+
  |     move chassis side and right to adjust the   |
  |     distance between the first pole and the in- |
  |     terface in local-y direction                |
  +-------------------------------------------------+
  | @attention : default acceptable err : +-derr(mm)|
  +-------------------------------------------------*/
/* chassis : µ×ÅÌ */

#if GROUND==RED
    #define F_SONIC 2   /* front sonic */
    #define S_SONIC 1   /* side sonic */
    static double std_fdis = 340;         /* when hanover auto robot, the distance between 
                                                   the second supersoinc and the second pole    */
    static double std_sdis = 414;         /* side distance */
    static const double derr = 3;               /* default error*/
    static const double std_poleDeep = 220;
    static const double std_overDesign =  530.0;      /* overdesign : Ô£Á¿ ,distance between front sonic
                                                        and pole*/
    static rbyCoef = 1; /* red and blue ground coefficient in y-direction */
#elif GROUND==BLUE
    #define F_SONIC 1   /* front sonic */
    #define S_SONIC 2   /* right sonic */
    static double std_fdis = 400;         /* when hanover auto robot, the distance between 
                                                   the second supersoinc and the second pole    */
    static double std_sdis = 414;         /* side distance */
    static const double derr = 3;               /* default error*/
    //static const double std_poleDeep = 220;
    static const double std_overDesign =  530.0;      /* overdesign : Ô£Á¿ ,distance between front sonic
                                                        and pole*/
    static int rbyCoef = -1;/* red and blue ground coefficient in y-direction */
#endif


extern double Pos_Pid(double err, double win, ePosPID_t dir, double max);
/* only in red field */
void pole_sonic(u32 data)
{
    double ferr = 0;
    double serr = 0;
    static double Vout[2] = {6.0, 7.0};
    rt_uint32_t key_value = 0;

    SONICx_ENABLE(data);
    while(1)
    {
        double fdis = (double)(sonic_data[F_SONIC-1].data);     /* front distance */
        double sdis = (double)(sonic_data[S_SONIC-1].data);     /* side  distance */
        
        //front supersonic adjust
        if(data&0x01 && sonic_data[F_SONIC-1].data<1000)
        {
            double v_max =2400;
            double basev = 250;
            double coef  = (v_max-basev)/2;
            
            buz_msg(0,0,0,200);
            ferr = fdis - std_fdis;
            Vout[0] = Pos_Pid(ferr, 1, eF_sonic, 1800.0);
            //Moto_Set_GSpeed(W_MOTOR1_OLD_ID, (int16_t)Vout[0]);
            //Vout[0] = Pos_Pid(ferr, 0.998, eF_sonic, 50.0);
            if(Vout[0]*coef+basev > v_max)
                W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, Motor_Pos[0]+Vout[0], v_max, CMD_SET_PSG);//CMD_SET_PSG
            else if(Vout[0]*coef+basev >0)
                W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, Motor_Pos[0]+Vout[0], basev+Vout[0]*coef, CMD_SET_PSG);//CMD_SET_PSG
            else if(Vout[0]*coef+basev >-v_max)
                W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, Motor_Pos[0]+Vout[0], -basev+Vout[0]*coef, CMD_SET_PSG);//CMD_SET_PSG
            else
                W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, Motor_Pos[0]+Vout[0], -v_max, CMD_SET_PSG);//CMD_SET_PSG
            
        }
        else if(data&0x01)
        {
            Moto_Set_GSpeed(W_MOTOR1_OLD_ID, 0);
            //anyKey(500, "front sonic can't see");
            //buz_msg(3,2,1,200);
        }
        else
        {Moto_Set_GSpeed(W_MOTOR1_OLD_ID, 0);}

            
        //side supersonic adjust
        if(data&0x02 && sonic_data[S_SONIC-1].data<1000)
        {
            //buz_msg(0,0,0,200);
            serr = sdis - std_sdis;
            Vout[1] = Pos_Pid(serr, 0.999, eS_sonic, 500.0);
            Speed_Y = rbyCoef * Vout[1];
        }
        else if(data&0x02)
        {
            SPEED_STOP;
            LCD_Clear();
            //anyKey(500, " side sonic can't see ");
            //buz_msg(1,2,3,200);
            //break;
        }
        else
        {SPEED_STOP;}
    
        
        /* show infomation */
        LCD_Clear();
        LCD_SetXY(0, 0); LCD_Printf("vout0:%f", Vout[0]);
        LCD_SetXY(0, 1); LCD_Printf("vout1:%f", Vout[1]);
        LCD_SetXY(0, 2); LCD_Printf("ferr:%f", ferr);
        LCD_SetXY(0, 3); LCD_Printf("serr:%f", serr);
        LCD_SetXY(0, 4); LCD_Printf("fdis:%f", fdis);
        LCD_SetXY(0, 5); LCD_Printf("sdis:%f", sdis);
        
        /* wait for key event */
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            SONICx_DISABLE(0x03);
            
            Vout[0] = Vout[1] = 0.0;
            
            Speed_Y = 0;
            Moto_Set_GSpeed(W_MOTOR1_OLD_ID, 0);
            //buz_msg(0,0,0,200);
            break;
        }
        
        {/* wait for task finish */
            double ferr, serr;
            ferr = fdis-std_fdis;
            serr = sdis-std_sdis;
            if(ferr<=derr && fdis>=-derr && serr<=derr && serr>=-derr)
            {
                msg("reach the handover position");
                Speed_Y = 0;
                Moto_Set_GSpeed(W_MOTOR1_OLD_ID, 0);
                //buz_msg(0,0,0,200);
                break;
            }
        }
        
        /* period : 30ms */
        Delay_ms(5*6);
    }//while(1)
}




static void pw1_pid(void)
{
	rt_uint32_t key_value;
//	static double coefy2a = 0.8;
    
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);     LCD_WriteString("=====sonic_test=====");
		LCD_SetXY(0, 1);    LCD_WriteString("1.front pid");
        LCD_SetXY(0, 2);    LCD_WriteString("2.f->pid");
        LCD_SetXY(0, 3);    LCD_WriteString("3.side pid");
        LCD_SetXY(0, 4);    LCD_WriteString("4.s->pid"); 
        LCD_SetXY(10, 1);   LCD_WriteString("5.fs pid");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:      pole_sonic(0x01);                break;
				case key2:      _input_pid(&sPos_Pid[eF_sonic]); break; 
                case key3:      pole_sonic(0x02);                break; 
                case key4:      _input_pid(&sPos_Pid[eS_sonic]); break;
                case key5:      pole_sonic(0x03);                break;
                case keyback:   return;
			}
		}
		Delay_ms(10);
	}
}



/**----------------------------------------------+
 |        sonic pole handover                    |
 +-----------------------------------------------+
 |  @attention : when call this function the     |
 |  angle of the robot mast be correct           |
 +-----------------------------------------------*/

/* step1 */
/* y-direction move until supersonic see first pole */
/* go to next position according to the first pole position */
#include "math.h"
void _step1(void)
{    
    double dis = 0;
    struct Point goal_point;
    gps_t gps_save = GPS;
    
    SONICx_ENABLE(0X03);
    
    Speed_Y = rbyCoef * -600;
    while((double)(sonic_data[F_SONIC-1].data)>600 || (double)(sonic_data[F_SONIC-1].data)<150)
    {
        LCD_Clear();
        LCD_SetXY(0, 0);
        LCD_Printf("_step2: not see pole");
        LCD_SetXY(0, 1);
        LCD_Printf("angle:%f", GPS.radian);
        Delay_ms(5);
    }
    /* check again */
    Delay_ms(30);
    dis = sonic_data[F_SONIC-1].data;
    if(dis<500 && dis>150)
    {
        /* be sure seeing the first pole */
        
        double l_disx = (dis + 220 - std_overDesign);   /* local delta distance - x_direction */
        double l_disy = -450*rbyCoef;                   /* local delta distance - y_direction */
        double theta = GPS.radian;
        
        Speed_Y = 0;
        msg("see pole, dis:%f", dis);
        
        /* local to global */
        goal_point = GPS.position;
        goal_point.x +=  l_disx*cos(theta) - l_disy*sin(theta);
        goal_point.y +=  l_disx*sin(theta) + l_disy*cos(theta);
        
        _set_keep(goal_point, gps_save.radian, 1000, 150, 5);
    }
}

/* the manual robot and the auto robot separate */
void sonic_pw1(void)
{
    SONICx_ENABLE(0x03);
    
    SPEED_STOP;
    msg("sonic pole part ,go?");
    _step1();
    SPEED_STOP;
    msg("go on adjust ?");
    
    /* adjust to handover position */
    pole_sonic(0x03);
    
    SONICx_DISABLE(0x03);
}

/* get auto robot */
void sonic_pw2(void)
{
    
}


void sonic_SDsave(int sonicx, const char *f_name, int ndata);
void sys_sonic(void)
{
	rt_uint32_t key_value;
    
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0); LCD_WriteString("====pole_test====");
		LCD_SetXY(0, 1); LCD_WriteString("1.step1");
        LCD_SetXY(0, 2); LCD_WriteString("2.xypid");
        LCD_SetXY(0, 3); LCD_WriteString("3.pw1");
        LCD_SetXY(0, 4); LCD_WriteString("4.dis");
        
        LCD_SetXY(10, 1); LCD_WriteString("5.sd save");
        LCD_SetXY(10, 2); LCD_WriteString("6.");
        LCD_SetXY(10, 3); LCD_WriteString("7.");
        LCD_SetXY(10, 4); LCD_WriteString("8.");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:  _step1();       break;
				case key2:  pw1_pid();      break;
                case key3:  sonic_pw1();    break;
                case key4:  
                    Input_DoubleValue(&std_fdis, "f dis");    
                    Input_DoubleValue(&std_sdis, "s dis");
                case key5:
                {
                    int _sonicx = 0 ;
                    Input_IntValue(&_sonicx, "_sonicx");
                    sonic_SDsave(_sonicx, "sonic_data", 500);
                }  
                break;
                case keyback:               return;
			}
		}
		Delay_ms(10);
	}
}


static FIL sonic_file[6];
/**--------------------------------------+
  | @attention :                         |
  | sonicx : 1~6                         |
  +--------------------------------------*/
#include "sonic.h"
#include "string.h"
void sonic_SDsave(int sonicx, const char *f_name, int ndata)
{
    FRESULT res;
    u8 _sonicx = 0;
    char *_f_name = f_namecrt(f_name);
    extern struct rt_thread *rt_current_thread;
    /* enable the sonic */
    _sonicx = 0x01<<(sonicx-1);
    SONICx_ENABLE(_sonicx);    
    
    res = f_open (&sonic_file[sonicx-1], _f_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    f_lseek(&sonic_file[sonicx-1], sonic_file[sonicx-1].fsize);
    if(res != FR_OK)
    {LCD_ERROR();   anyKey(0, "file open err:%d", res); return;}
    
    SD_Printf2(&sonic_file[sonicx-1], "-------------------------------------------");
    SD_Printf2(&sonic_file[sonicx-1], "-->date:%s", __DATE__);    
    SD_Printf2(&sonic_file[sonicx-1], "-->time:%s", __TIME__);
    SD_Printf2(&sonic_file[sonicx-1], "-->thread:%s", rt_current_thread->name);
    SD_Printf2(&sonic_file[sonicx-1], "sonic%d data", sonicx);
    while(ndata-- >0)
    {
        SD_Printf2(&sonic_file[sonicx-1], "%d,", sonic_data[sonicx-1].data);
        Delay_ms(5*6);
        
        LCD_Clear();
        LCD_SetXY(0, 0);  LCD_Printf("sonic data saving : %d", sonic_data[sonicx-1].data);
    }
    SD_Printf2(&sonic_file[sonicx-1], "-------------------------------------------");
    SD_Printf2(&sonic_file[sonicx-1], "");
    SD_Printf2(&sonic_file[sonicx-1], "");
    
    f_close(&sonic_file[sonicx-1]);
    f_timestamp(_f_name);
    
    SONICx_DISABLE(_sonicx);
}

