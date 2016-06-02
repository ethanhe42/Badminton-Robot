
#include "includes.h"
#include "fxh_misc.h"

static void sd_demo(void);


extern int vsprintf(char *string,const char *format, va_list param);
/*------------------------------------------* 
 | show string then wait for key event      |
 *------------------------------------------*/
int msg_debug = 0;
u8 msg(const char *fmt,... )
{
    static u32 key_val ;
    u8   return_value = 0;
    va_list ap;
    char string[128];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);
    LCD_Clear();
    LCD_SetXY(0,0);     LCD_WriteString(string);
    LCD_SetXY(0,3);     LCD_Printf("in func[%s]", __func__);
    
    if(!msg_debug)
        return 0;

    SPEED_STOP;    
    LCD_SetXY(0,4);     LCD_WriteString("press any key");
    
    while( 1 )
    {
        if(rt_mb_recv(&Mb_Key, (rt_uint32_t*)&key_val, 2 )==RT_EOK)
            break;
        rt_thread_delay( 5 );
    }
    
    return  return_value ;
}



char *f_nameDeal(char *p_char)
{
	char *save = p_char;
 	while(*p_char++!=0)
 	{
 		if(*p_char==' ' || *p_char==':' || *p_char=='-' || *p_char=='(' || *p_char==')')
 			*p_char = '_'	;
 	}
	return save;
}

char *f_namecrt(const char *f_name)
{
    static char buf[128] = {0};
    int i=0;

    for(i=0; i<sizeof(buf)/sizeof(buf[0]); i++)
        buf[i]=0;

    sprintf(buf, "%s_%d.txt", f_name, rt_tick_get()/200);
    return f_nameDeal(&buf[0]);
}


/*------------------------------------------* 
 | show string then wait for key event      |
 *------------------------------------------*/
u8 anyKey(u32 delay_ms, const char *fmt,... )
{
    static u32 key_val ;
    u8   return_value = 0;
    va_list ap;
    char string[128];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);
    
    LCD_Clear();
    LCD_SetXY(0,0);     LCD_WriteString(string);     
    
    if(delay_ms==0)
        while( 1 )
        {
            LCD_SetXY(0,0);     LCD_WriteString(string);   
            LCD_SetXY(0,4);     LCD_WriteString("press any key");
            
            if(rt_mb_recv(&Mb_Key, (rt_uint32_t*)&key_val, 2 )==RT_EOK)
                return (u8)key_val;
            
            rt_thread_delay( 20 );
        }
    else
    {
        int tickCnt=0;
        while(tickCnt*5<delay_ms)
        {
            tickCnt ++;
            LCD_Clear();
            LCD_SetXY(0,0);     LCD_WriteString(string);
            LCD_SetXY(0,4);     LCD_WriteString("wait a moment");
            
            if(rt_mb_recv(&Mb_Key, (rt_uint32_t*)&key_val, 2 )==RT_EOK)
                return (u8)key_val;
            rt_thread_delay(1);
        }
    }
        
    return  return_value ;
}



/**------------------------------------------------*
 |  set keep  @author : owen @modified by fxh_bool |
 *-------------------------------------------------*/
typedef struct PID pid_t;

#include "math.h"
extern double Pos_Pid(double err, double win, ePosPID_t dir, double max);
void _set_keep(
        point_t pt,         /* reference point  */
        double  aim_rad,    /* aim angle        */
        double  maxv,       /* maximum   speed  */
        double  maxa,       /* maximum   a speed*/
        double  err         /* acceptable err   */
    )
{
	double lineangle;
	double speed=0;
	double speed_x=0,speed_y=0;					//世界坐标里的速度与分速度
	double speed_robot_x,speed_robot_y;			//转换到机器人坐标的分速度
	double phi;
	
	double error_angle,error_dis;				//角度误差与距离误差
	double vout_a=0;							//PID调整后角度输出值
	double vout_d=0;							//PID调整后距离输出值
	
	PID_Clear();
	
	while(1)
	{
		lineangle = GetLineAngle(GPS.position, pt);	//求出该路径线段的倾角
		
		//求角度误差与距离误差
		error_angle = aim_rad-GPS.radian;
		error_dis   = GetLength(GPS.position,pt);
		
		if(error_angle>pi)  error_angle-=2*pi;
		if(error_angle<-pi) error_angle+=2*pi;
		
		//PID调整
		vout_a = 1000*Pos_Pid(error_angle, 1, eA_Keep, 100);
		vout_d = Pos_Pid(error_dis,1, eD_Keep, 1000);
		
		//限最大转速
		if(vout_a > maxa) vout_a = maxa;
		else if(vout_a < -maxa) vout_a = -maxa;
		
		speed = vout_d;
		
		//限最大速
		if(speed > maxv) speed = maxv;
		else if(speed < -maxv) speed = -maxv;
		
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
		Speed_Rotation = vout_a;
		
		//LCD显示
		LCD_Clear();
		
        LCD_SetXY(0,0);     LCD_WriteString("A:");      LCD_WriteDouble(GPS.radian);
		LCD_SetXY(10,0);    LCD_WriteString("X:");      LCD_WriteDouble(GPS.position.x);
		LCD_SetXY(0,1);     LCD_WriteString("Y:");      LCD_WriteDouble(GPS.position.y);
		LCD_SetXY(0,2);     LCD_WriteString("err_a:");  LCD_WriteDouble(error_angle);
		LCD_SetXY(0,3);     LCD_WriteString("err_d:");  LCD_WriteDouble(error_dis);
		LCD_SetXY(0,4);     LCD_WriteString("vout_a:"); LCD_WriteDouble(vout_a);
		LCD_SetXY(0,5);     LCD_WriteString("vout_d:"); LCD_WriteDouble(vout_d);
		LCD_SetXY(0,6);     LCD_WriteString("speedx:"); LCD_WriteDouble(speed_x);//
		LCD_SetXY(0,7);     LCD_WriteString("speedy:"); LCD_WriteDouble(speed_y);
		LCD_SetXY(0,8);     LCD_WriteString("line_a:"); LCD_WriteDouble(lineangle);
		
		Delay_ms(5);
		
		if(GetLength(GPS.position,pt)<err && (GPS.radian-aim_rad<0.005 || GPS.radian-aim_rad<-0.005))   //0.002大致是0.1°
		{
			SPEED_STOP;
			break;
		}
	}
}


extern struct PID sPos_Pid[];

void sys_keep(void)
{
    rt_uint32_t key_value;
    static double v_d=300, v_a=200;
    static double dx=400, dy=400;
                    
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);  LCD_WriteString("====laser_ladder====");
		LCD_SetXY(0, 1); LCD_WriteString("1.set  va max");
		LCD_SetXY(0, 2); LCD_WriteString("2.keep x");
		LCD_SetXY(0, 3); LCD_WriteString("3.keep y");
		LCD_SetXY(0, 4); LCD_WriteString("4.keep xy");
		LCD_SetXY(0, 5); LCD_WriteString("0.keep ap");

		LCD_SetXY(10, 1); LCD_WriteString("5.keep dp");
		LCD_SetXY(10, 2); LCD_WriteString("6.keep di");
		LCD_SetXY(10, 3); LCD_WriteString("7.keep dd");
		LCD_SetXY(10, 4); LCD_WriteString("8.keep ai");
		LCD_SetXY(10, 5); LCD_WriteString("9.keep ad");
        
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
                    Input_DoubleValue(&v_d, "v_d");
                    Input_DoubleValue(&v_a, "v_a");
				break;
                
                case key2:   /* set keep x */
                {
                    
                    struct Point g_pos = GPS.position;
                    Input_DoubleValue(&dx, "dx");
                    g_pos.x += dx;
                    _set_keep(g_pos, 0, v_d, v_a, 5); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 4); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 3); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 2);
                } 
                break;
                
                case key3:   /* set keep y */
                {
                    
                    struct Point g_pos = GPS.position;
                    Input_DoubleValue(&dy, "dy");
                    g_pos.y += dy;
                    _set_keep(g_pos, 0, v_d, v_a, 5); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 4); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 3); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 2);
                }    
				break;
                
                case key4:   /* set keep x-y */
                {
                    struct Point g_pos = GPS.position;
                    Input_DoubleValue(&dx, "dx");
                    Input_DoubleValue(&dy, "dy");
                    g_pos.x += dx;
                    g_pos.y += dy;
                    
                    _set_keep(g_pos, 0, v_d, v_a, 5); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 4); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 3); Delay_ms(300);
                    _set_keep(g_pos, 0, v_d, v_a, 2);
                } 
				break;
                
                case key0: Input_DoubleValue(&sPos_Pid[eA_Keep].p, "ap"); break;
                case key5: Input_DoubleValue(&sPos_Pid[eD_Keep].p, "dp"); break;
                case key6: Input_DoubleValue(&sPos_Pid[eD_Keep].i, "di"); break;
                case key7: Input_DoubleValue(&sPos_Pid[eD_Keep].d, "dd"); break;
                case key8: Input_DoubleValue(&sPos_Pid[eA_Keep].i, "ai"); break;
                case key9: Input_DoubleValue(&sPos_Pid[eA_Keep].d, "ad"); break;
                case keyback: return;
			}
		}
		Delay_ms(10);
	}
}

// void _set_a(void)
// {

// }

/**--------------------------------------+
  | open or close the debug(ie.msg) port |
  +--------------------------------------*/
void sys_debug(void)
{
    static rt_uint32_t key_value;
    
    while( 1 )
    {
        LCD_Clear();
        LCD_SetXY(0, 0);    LCD_WriteString("===debug port===");
        LCD_SetXY(0, 1);    LCD_Printf("1.msg  debug : %d", msg_debug);
        LCD_SetXY(0, 2);    LCD_Printf("2.buzz debug : %d", buz_debug);
        
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            switch(key_value)
            {
                case key1: if(msg_debug) msg_debug=0; else msg_debug = 1;  break;
                case key2: if(buz_debug) buz_debug=0; else buz_debug = 1;  break;
                case keyback:       return;
            }
        }
        Delay_ms(5);
    }
}

/*---------------------------------------+
  | test easyline                        |
  +--------------------------------------*/
void testEasyLine(void)
{
    static rt_uint32_t key_value;
    point_t endPt;
//    double  start_a = GPS.radian;
    endPt.x += GPS.position.x + 400*cos(pi/2+GPS.radian);
    endPt.y += GPS.position.y + 400*sin(pi/2+GPS.radian);
    while(1)
    {
        LCD_Clear();
        LCD_SetXY(0,0); LCD_Printf("dx:%d, dy%d", (s32)(GPS.position.x-endPt.x), (s32)(GPS.position.y-endPt.y));
        EasyLine(endPt, GPS.radian, 400);
        Delay_ms(5);
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
            break;
    }
}


/**--------------------------------------+
  | misc                                 |
  +--------------------------------------*/


#include "time.h"


void sys_mtpwm(void);
void sys_misc(void)
{
    static rt_uint32_t key_value;
    
    while( 1 )
    {
        LCD_Clear();
        LCD_SetXY(0, 0);    LCD_WriteString("===  misc  ===");
        LCD_SetXY(0, 1);    LCD_Printf("1.sd_demo");
        LCD_SetXY(0, 2);    LCD_Printf("2.ttime");
        LCD_SetXY(0, 3);    LCD_Printf("3.mtpwm");
        
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            switch(key_value)
            {
                case key1: sd_demo();       break;
                case key2: sys_ttime();     break;
                case key3: sys_mtpwm();     break;
                case keyback:               return;
            }
        }
        
        Delay_ms(15);
    }
}


/**-----------------------------------------+
  |  create,write,close,timestame template            |
  +-----------------------------------------*/

static void sd_demo(void)
{
    static FIL fil_demo;
    FRESULT res;
    
    res = f_open (&fil_demo, "f_demo.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);    //FA_OPEN_ALWAYS就是如果文件不存在，就创建
    res = f_lseek(&fil_demo, fil_demo.fsize);   // 接着往下写，不是从头写
    res = (FRESULT)f_printf(&fil_demo, "fdemo.txt");
    res = f_close(&fil_demo);                   //只有有了关闭操作，文件才会完整
    res = f_timestamp("f_demo.txt");            //给文件打上时间戳（在文件关闭的前提下）
    SD_RES(res);
}




/**-----------------------------------------------------------+
  | 功能：直接给电机赋pwm波的函数的接口                   |
  | 参数：pwm
  +-----------------------------------------------------------*/
static void mt_pwm(ID_NUMDEF ID_NUM, s8 pwm)
{
    int i=0;
    union { u8 u8v; s8 s8v;}vout;

    vout.s8v = pwm;
    if(pwm>100 || pwm<-100)
    {
        msg("%d", pwm);
        LCD_ERROR();
        msg("param err in [%s]", __func__);
    }
    for(i=0; i<8; i++)
        *(Can_Database[HASH_TABLE[ID_NUM]].Data_ptr+i) = 0;
	
    *(Can_Database[HASH_TABLE[ID_NUM]].Data_ptr+6) = vout.u8v;
	*(Can_Database[HASH_TABLE[ID_NUM]].Data_ptr+7) = 0x06;   
	
	Write_Database(ID_NUM);
}


    /*--------------------------------------------------+
    |                                                   |
    | motot line        ^                               |
    |                v  |                               |
    |    v2_____        |                               |
    |     /     \       |                               |
    |    /       \      |                               |
    |   /         \     |                               |
    |  /v1         \    |                               |
    |               \   |                               |
    |  --------------\--|----------> 位置 p            |
    |  p1  p2   p3    \ |  p4                           |
    |                  \|                               |
    |                   \                               |
    |                   |\                              |
    |                   | \                             |
    |                   |  \ v3                         |
    |                                                   |
    +---------------------------------------------------*/

/**------------------------------------------------------+
   |加速阶段                | 减速阶段                 |
   |   vx - v1     pnow - p1 |    vx - v3     p4 - pnow  |
   |  ————————— = —————————— |   ————————— = ——————————  |
   |   v2 - v1     p2   - p1 |    v2 - v3     p4 - p3    |
   +-----------------------------------------------------+
*/
//motor line 

/**--------------------------------------------------------------

参数要满足的条件：
    1、 三个速度的值要同号
    2、 start_ratio 、 end_ratio 以及两者之和的范围：0.0 ~ 1.0
    3、 v1 v2 v3 正负号相同且 范围：-100~100
    4、 ID_NUM 范围:W_MOTOR1_OLD_ID ~ W_MOTOR6_OLD_ID

注意：在每个阶段停留的时间超过8s将显示超时错误，
      按键后自动跳出该函数，这个延时也可以改
----------------------------------------------------------------*/
static int mt_line_timeout = 20;   /* 单位是 s */
void mt_line( ID_NUMDEF ID_NUM, double startp, double endp, double start_ratio, double end_ratio, s8 v1, s8 v2, s8 v3)
{
    double  p1=0, p2=0, p3=0, p4=0;
    s8      vout=0;
    int     timCnt = 0;
    double  pos = 0;
    int     dir = 0;
    
    /* 防止除0错误 */
    if(start_ratio<0.05)    startp = 0.05;
    if(end_ratio<0.05)      endp   = 0.05;
    
    p1 = startp;
    p4 = endp;
    p2 = p1 + (endp-startp)*start_ratio;
    p3 = p4 - (endp-startp)*end_ratio;    
    
    /* param check */
    if(start_ratio<0 || start_ratio>1 || end_ratio<0 || end_ratio>1)    {LCD_ERROR(); return;}
    if(v1<-100 || v1>100 || v2<-100 || v2>100 || v3<-100 || v3>100)     {LCD_ERROR(); return;}
    if(start_ratio+end_ratio >1)                                        {LCD_ERROR(); return;}
    if(v1*v2<0 || v2*v3<0 || v1*v3<0)                                   {LCD_ERROR(); return;}
    if(ID_NUM<W_MOTOR1_OLD_ID || ID_NUM>W_MOTOR6_OLD_ID)                {LCD_ERROR(); return;}


    if(endp-startp > 0)         dir = 1;
    else if(endp-startp < 0)    dir = -1;
    else                        {LCD_ERROR(); return;}

    /* stage 1 : speed up */
    timCnt = 0;
    while(1)
    {
        pos = Motor_Pos[ID_NUM-W_MOTOR1_OLD_ID];
        if((dir>0 && pos>p2) || (dir<0 && pos<p2))
        {
           break;
        }
        else
        {
            vout = (s8)( (s8)(v2-v1)*(pos-p1)/(p2-p1) + v1 );
            mt_pwm(ID_NUM, vout);
        }
        
        timCnt ++;
        if(timCnt>200*mt_line_timeout)
        {anyKey(0, "timeout in [%s]", __func__); return;}
        
        /* lcd show */
        LCD_Clear();
        LCD_SetXY(0, 0); LCD_Printf("stage 1");
        LCD_SetXY(0, 1); LCD_Printf("vout:%d", vout);
        LCD_SetXY(0, 2); LCD_Printf("pos:%f", pos);
        
        Delay_ms(5);
    }
    
    /* stage 2 : constant speed */
    timCnt = 0;
    while(1)
    {
        pos = Motor_Pos[ID_NUM-W_MOTOR1_OLD_ID];
        if((dir>0 && pos>p3) || (dir<0 && pos<p3))
        {break;}
        else
        {
            vout = v2;
            mt_pwm(ID_NUM, vout);
        }
        timCnt ++;
        if(timCnt>200*mt_line_timeout)
        {anyKey(0, "timeout in [%s]", __func__); return;}
        
        
        /* lcd show */
        LCD_Clear();
        LCD_SetXY(0, 0); LCD_Printf("stage 2");
        LCD_SetXY(0, 1); LCD_Printf("vout:%d", vout);
        LCD_SetXY(0, 2); LCD_Printf("pos:%f", pos);
        
        Delay_ms(5);
    }
    
    /* stage 3 : slow down */
    timCnt = 0;
    while(1)
    {
        pos = Motor_Pos[ID_NUM-W_MOTOR1_OLD_ID];
        if((dir>0 && pos>p4) || (dir<0 && pos<p4))
        {break;}
        else
        {
            vout = (s8)( (s8)(v2-v3)*(p4-pos)/(p4-p3) + v3 );
            mt_pwm(ID_NUM, vout);
        }
        
        timCnt ++;
        if(timCnt>200*mt_line_timeout)
        {anyKey(0, "timeout in [%s]", __func__); return;}        
        
        
        /* lcd show */
        LCD_Clear();
        LCD_SetXY(0, 0); LCD_Printf("stage 3");
        LCD_SetXY(0, 1); LCD_Printf("vout:%d", vout);
        LCD_SetXY(0, 2); LCD_Printf("pos:%f", pos);
        
        Delay_ms(5);
    }
    
}

//ss_up



void sys_mtpwm(void)
{
//    int i=0;
    static rt_uint32_t key_value;
    int mtx=0;
    int idmtx=0;
    
    Input_IntValue(&mtx, "motorx:1~3");
    if(mtx<1 || mtx>3)
        LCD_ERROR();
    idmtx = W_MOTOR1_OLD_ID + mtx - 1;
    
    while( 1 )
    {
        LCD_Clear();
        LCD_SetXY(0, 0);    LCD_Printf("1.dir test");
        LCD_SetXY(0, 1);    LCD_Printf("2.ss test");
        LCD_SetXY(0, 7);    LCD_Printf("pos:%f mtx:%d", (Motor_Pos[mtx-1]), mtx);
        
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            switch(key_value)
            {
                case key1:                  
                    mt_pwm((ID_NUMDEF)idmtx, 30);
                    Delay_ms(200);
                    mt_pwm((ID_NUMDEF)idmtx, 0);   
                break;
                case key2:                 
                {
                    mt_line((ID_NUMDEF)idmtx, 0, 30, 0.3, 0.3, 5, 40, 3);
                } 
                Moto_Stop((ID_NUMDEF)idmtx);
                break;
                case keyback:               
                return;
            }
        }
        Delay_ms(15);
    }    
}



