
#include "lightboard1.h"
#include "includes.h"
#include "fxh_bool.h"
/*
 * 函数名: 
 * 描  述: 
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
 
#include "pid.h"
#include "pid1.h"
#include "math.h"
static double lb_vy = 0;
void LB_Test1(u32 data, gps_t gps_goal)
{
//    const static double coefy2a = 0.08; /* coefficient: y speed to angle */ 
    static rt_uint32_t key_value;
    
//	double phi = 0;    //GPS.radian
//	double speed_x,speed_y;				        //世界坐标里的速度与分速度
//	double speed_robot_x,speed_robot_y;			//转换到机器人坐标的分速度
	
	double error_D;
    double lb1_last=(double)LB_Count(1), lb2_last=(double)LB_Count(2);
//     int lb1_stay = 0;
//     int lb2_stay = 0;
	double error_A;
	double Vout_A=0;							//PID调整后角度输出值
 	double Vout_D=0;							//PID调整后距离输出值
    
//	double v_err_x,v_err_y;						//PID调整速度的分量
	
	while(GPS.position.y-gps_goal.position.y > 30)
	{
        if(   (double)LB_Count(1)-lb1_last> 2 ||(double)LB_Count(1)-lb1_last<-2 || 
                (double)LB_Count(2)-lb2_last> 2 || (double)LB_Count(2)-lb2_last<-2    )
        {
            error_D = lb1_last+lb2_last-2*12;
            error_A = lb2_last-lb1_last;
            msg("1:%f 2:%f", (double)LB_Count(1)-lb1_last, (double)LB_Count(2)-lb2_last);
        }
		else
        {
            lb1_last = LB_Count(1);
            lb2_last = LB_Count(2);
            error_D = (double)LB_Count(1)+(double)LB_Count(2)-2*12;
            error_A = (double)LB_Count(2)-(double)LB_Count(1);
        }
		
        //comment by fxh_bool 4-9
		//Vout_D = DistancePID(error_D,LB_Pid_List[0]);
		//Vout_A = AnglePID(error_A,LB_Pid_List[1]);
        
        //added by fxh_bool 4-9
        Vout_D = Pos_Pid(error_D, 0.998, eD_LB, 1500);
        Vout_A = -Pos_Pid(error_A, 0.998, eA_LB, 300);
		
		//PID调整速度的分量
//		v_err_x =  Vout_D*cos(phi);
//		v_err_y = -Vout_D*sin(phi);
		
// 		speed_x = v_err_x;
// 		speed_y = v_err_y;
		
		//将世界坐标的速度转换为机器人局部坐标的速度
// 		speed_robot_x =  speed_x*cos(phi) + speed_y*sin(phi);
// 		speed_robot_y = -speed_x*sin(phi) + speed_y*cos(phi);
		
		//为四轮赋速度
        
        if(data&0x01)
            Speed_X = Vout_D;
		
        //Speed_Y = 200;
		
        if(data&0x02)
            Speed_Rotation = -0.073*Speed_Y + Vout_A;
        else
            Speed_Rotation = 0;
        
        if(data&0x03)
            Speed_Y = lb_vy;
		
		LCD_SetXY(0,0);
		LCD_Printf("LB1:%d  LB2:%d", LB_Count(1),LB_Count(2));
		LCD_SetXY(0,1);
		LCD_Printf("LB1+LB2: %d",LB_Count(1)+LB_Count(2));
		LCD_SetXY(0,2);
		LCD_Printf("LB1-LB2: %d",LB_Count(1)-LB_Count(2));
		LCD_SetXY(0,3);
		LCD_Printf("error_D:%g",error_D);
		LCD_SetXY(0,4);
		LCD_Printf("error_A:%g",error_A);		
		LCD_SetXY(0,5);
		LCD_Printf("Vout_D:%g",Vout_D);
		LCD_SetXY(0,5);
		LCD_Printf("Vout_A:%g",Vout_A);
        LCD_SetXY(0,6);
		LCD_Printf("Speed_X:%f", Speed_X);
		
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            Speed_X = 0;
            break;
        }
        Delay_ms(5);
	}
}

static void lb_sdsave(void)
{
    static FIL fil_lb;
    int i=0;
    FRESULT res;
    res = (FRESULT)f_open (&fil_lb, "f_lb.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE); 
    res = (FRESULT)f_lseek(&fil_lb, fil_lb.fsize); 
    
    res = (FRESULT)f_printf(&fil_lb, "\n\n\n--------------------------------------\n");
    SD_RES(res);
    while(i++ <2000)
    {   
        LCD_Clear();
        LCD_SetXY(0, 0);    LCD_Printf("sonic saving");
        f_printf(&fil_lb, "%d,%d;\n", LB_Count(1), LB_Count(2));
        Delay_ms(15);
    }
    res = (FRESULT)f_printf(&fil_lb, "--------------------------------------\n");
    
    res = f_close(&fil_lb);   
    res = f_timestamp("f_demo.txt"); 
}

#include "pid1.h"
void sys_lb(void)
{
	rt_uint32_t key_value;
//	static double coefy2a = 0.8;
    
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0); LCD_WriteString("=====fxh_test=====");
		LCD_SetXY(0, 1); LCD_WriteString("1.x");
        LCD_SetXY(0, 2); LCD_WriteString("2.xpid");
        LCD_SetXY(0, 3); LCD_WriteString("3.a");
        LCD_SetXY(0, 4); LCD_WriteString("4.apid");
        LCD_SetXY(10, 1); LCD_WriteString("5.xa");
        LCD_SetXY(10, 2); LCD_WriteString("6.lb_save");
        LCD_SetXY(10, 3); LCD_WriteString("7.by_init");
        LCD_SetXY(10, 4); LCD_WriteString("8.vy");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1://x
                {
                   void LB_Test1(u32 data, gps_t gps_gpal);
                   gps_t gps = GPS;
                   gps.position.x +=100000;
                   gps.position.y +=100000;
                        LB_Test1(0x01, gps);
					break;
                }
				case key2 ://xpid
                    _input_pid(&sPos_Pid[eD_LB]);
                break;
                    
                case key3 ://a
                {
                   gps_t gps = GPS;
                   gps.position.x +=100000;
                   gps.position.y +=100000;
                   LB_Test1(0x02, gps);
                }
                break;
                
                case key4 ://apid
                        _input_pid(&sPos_Pid[eA_LB]);
                break;
                
                case key5://ax
                {
                   gps_t gps = GPS;
                   gps.position.x +=100000;
                   gps.position.y +=100000;
                        LB_Test1(0x03, gps);
					break;
                }					
                case key6:
                    lb_sdsave();
                    break;
                case key7:
                    Com_SendByte(2,0x07);
					Com_SendByte(5,0x07);
                    break;
                case key8:
                   Input_DoubleValue(&lb_vy, "vy");
                    break;
				case keyback:
                return;
			}
		}
		Delay_ms(10);
	}
}


// const static double std_lx = 0;    /* standard local x */
// const static double std_ly = 0;
// const static double std_la = 0;

// const static double std_lba = 5.0;  /* standard lightboard angle */
// const static double std_lbd = 5.0;  
void lb_ss(void)
{
//     gps_t gps_save = GPS;
//     gps_t gps_goal = GPS;
//     
//     gps_goal.position.x +=  std_lx*cos(gps_save.radian) - std_ly*sin(gps_save.radian);
//     gps_goal.position.y +=  std_lx*sin(gps_save.radian) + std_ly*cos(gps_save.radian);
//     gps_goal.radian     +=  std_la;
    
    /* init */
    Com_SendByte(2,0x07);
    Com_SendByte(5,0x07);
}
