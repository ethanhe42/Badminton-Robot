
#include "includes.h"
#include "laser1.h"
#include "math.h"



/*--------------------------------------------*
 |  variables                                 |
 *--------------------------------------------*/
static const u32 nbuf = 200;
static lsPos_t  ls_buf[els_n][nbuf];
static lsTask_t lsTask[els_n];

/**@find
 +--------------------------------------------*
 |  find the obstacle(bars) and save data     |
 *--------------------------------------------*/
static eLsErr_t _find(
        u8          laserx,
        elsPos_t    barx,                    /* bar x */
        double      dmin,                    /* min distance */
        double      dmax,                    /* min distance */
        double      range,                   /* maximum distance the car moves (millimeter) */
        int         sd_save,                  /* sd save or not */
        double      speed
    )
{
    double ls_dis = 0;           /* laser distance */
    int see_flag  = 0;           /* the laser see the obstacle ? */
    int index     = 0;
    lsPos_t *p_lspos = &ls_buf[barx][0];
    point_t pt_start = GPS.position;
    
    u32 lsErrCnt=0;
    
    for( ; ; Delay_ms(5))
    {
        LCD_Clear();
        LCD_SetXY(0, 0);
        
        /* read data from rangefinder */
        //ls_dis = (double)LaserRead(laserx);  

        /* judge if the laserfinder is OK */
        if(ls_dis==0)    lsErrCnt++;
        if(lsErrCnt>20)  return lsErr_laser;
        
        /* show infomation */
        if(index==0)
        {   /* show message before laser see the obstacle */
            LCD_Clear();
            LCD_SetXY(0, 0); LCD_Printf("not see obstacle");
            LCD_SetXY(0, 1); LCD_Printf("gps.a:%f", GPS.radian*57.3);
            LCD_SetXY(0, 2); LCD_Printf("range:%f", GetLength(GPS.position, pt_start));
        }
        else
        {
            LCD_Clear();
            LCD_SetXY(0, 0); LCD_Printf("in [%s] dis:%d", __func__, (s32)ls_dis);
        }
        
        if(ls_dis>dmin && ls_dis<dmax)
        {
            /* the laser see the obstacle */
            
            LCD_Clear();
            LCD_Printf("obstacle is in the laser view now!");
            
            see_flag = 1;
            p_lspos->dis = ls_dis;
            p_lspos->pt  = GPS.position;
            p_lspos->a   = GPS.radian;
            p_lspos ++;
            index   ++;
            
            if(index>=nbuf)
            {
                Speed_X = Speed_Y = 0;
                LCD_ERROR();
                msg("point num over %d", nbuf);
                return lsErr_bufof;
            }
            
            continue;
        }
        
        /* cannot see ladder again */
        if(see_flag==1)
        {
            
            LCD_Clear();
            LCD_Printf("obstacle is out the laser view now again! ");
            
            Speed_X = Speed_Y = 0;
            //p_disPt = get_minDisPt(&p_disPt[0], index);
            lsTask[barx].mid = &ls_buf[barx][index/2]; 
            lsTask[barx].num = index;
            
            /* sd_printf Point infomation ? */
            if(sd_save)
            {
                eLsErr_t res;
                 if(res!=lsErr_OK)
                 {}
            }
               
            msg("see obstacle->dis:%d index:%d", (s32)lsTask[barx].mid->dis, index);
            break;
        }
        
        /* out range */
        if(GetLength(GPS.position, pt_start)>=range)
        {
            /* the laser didn't see the ladder */
            Speed_X = Speed_Y = 0;
            msg("Err: out in %dmm range", (int)range);
            return lsErr_outRange;
        }
    }
    
    LCD_Clear();
    LCD_Printf("leave [%s]", __func__);
    return lsErr_OK;
}

#if GROUND==BLUE
static u8 laserx = 1;

static ls1p_t _ls1p[] = 
{
    //range  phi                stdx    	stdy    dmin    dmax    vx  vy      ebuf        sd   dia_bar
   { 950,    135.0*pi/180,      1000-200,   800+95, 500,    1350,   0,  600,    els_pw1,    1,   32    },   /* polewalk */
   { 800,    135.0*pi/180,      832+130,    245,    700,    1200,   0,  500,    els_ld1,    1,   50    },     /* ladder */
};
#elif GROUND==RED

static u8 laserx = 2;
    //range  phi                stdx    	stdy    dmin    dmax    vx  vy      ebuf        sd   dia_bar
   { 950,    -45.0*pi/180,      1000-200,   800+95, 500,    1350,   0,  600,    els_pw1,    1,   32    },   /* polewalk */
   { 800,    -45.0*pi/180,      832+130,    245,    700,    1200,   0,  500,    els_ld1,    1,   50    },     /* ladder */
#endif

void lssp(int i)
{
    SPEED_STOP;
    anyKey(0, "this func has been deleted");
}

/**@ls1p
  +-------------------------------------------------+
  |     general func for 1 point task               |
  +-------------------------------------------------*/
eLsErr_t ls1p( els1p_t _1px)
{
    ls1p_t *ls1px = &_ls1p[_1px];
    lsPos_t *p = NULL;
    eLsErr_t err = lsErr_OK;
    
    /* 如果是在楼梯处要多找一个点 */
    if(_1px==e1p_ld || _1px==e1p_pw)
        err = _find(laserx,(elsPos_t)ls1px->ebuf, ls1px->dmin, ls1px->dmax, ls1px->rg, ls1px->sd, ls1px->vy);
    
    err = _find(laserx, (elsPos_t)ls1px->ebuf, ls1px->dmin, ls1px->dmax, ls1px->rg, ls1px->sd, ls1px->vy);
    if(err!=lsErr_OK)  return  err;
    else    p = lsTask[ls1px->ebuf].mid;
    {
        point_t goal_pt;
        double  err_a  = 0;                 /* unit : radian */
        double  aim_a  = 0;
        double  l_disx = 0;                 /* local delta distance - x_direction   */
        double  l_disy = 0;                 /* local delta distance - x_direction   */
        
        err_a = p->a - ls1px->phi;
        goal_pt = p->pt; 
        l_disx = (p->dis*cos(err_a) - ls1px->stdx) - sin(err_a)*430;
        l_disy = ls1px->stdy + p->dis*sin(err_a) + ls1px->dia_bar *err_a + 100*sin(err_a);
        
        aim_a = ls1px->phi;
        goal_pt.x +=  l_disx*cos(aim_a) - l_disy*sin(aim_a);
        goal_pt.y +=  l_disx*sin(aim_a) + l_disy*cos(aim_a);

        /* 给 mb_Sensor 发邮箱表示现在已经找着点了 */
        if(_1px==e1p_ld)   rt_mb_send(&Mb_SensorEnd, eLsld_find);
        if(_1px==els_pw1)   rt_mb_send(&Mb_SensorEnd, eLspw_find);
        
        _set_keep(goal_pt, aim_a, 600, 150, 10);
        Delay_ms(200);
        _set_keep(goal_pt, aim_a, 600, 150, 3);
    }
    
    /* 给 MB_SensorEnd 发邮箱表示激光测距已经校准完成 */
    if(_1px==e1p_ld)   rt_mb_send(&Mb_SensorEnd, eLsld_end);
    if(_1px==els_pw1)   rt_mb_send(&Mb_SensorEnd, eLspw_end);

    
    return lsErr_OK;
}



/**@sys_lssp
  +--------------------------------------------------+
  | menu func , test single point task               |
  +--------------------------------------------------*/
void sys_lssp(void)/* single point */
{
    rt_uint32_t key_value;
    
	while(1)    
	{
		LCD_Clear();
		LCD_SetXY(0, 0); LCD_WriteString("==laser one-point==");
		LCD_SetXY(0, 1); LCD_WriteString("1.ls1p_pl");
		LCD_SetXY(0, 2); LCD_WriteString("2.ls1p_ld");
        LCD_SetXY(0, 3); LCD_WriteString("3.");
        LCD_SetXY(0, 4); LCD_WriteString("4.pw v");
        LCD_SetXY(10,1); LCD_WriteString("5.ld v");
        
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1: ls1p(e1p_pw);    break;
				case key2: ls1p(e1p_ld);    break;
                case key3:                  break;
                case key4: Input_DoubleValue(&_ls1p[e1p_pw].vy, "pw vy");    break;
                case key5: Input_DoubleValue(&_ls1p[e1p_ld].vy, "ld vy");    break;
                case keyback:               return;
			}
		}
		Delay_ms(10);
	}
}


/**@walkground
  +==================================================================+
  | laser walk ground                                                |
  +==================================================================*/

eLsErr_t ls1pwg(els1p_t _1px)
{
    //int i=0;
    ls1p_t *ls1px = &_ls1p[_1px];
    lsPos_t *p = NULL;
    eLsErr_t err = lsErr_OK;
    
    Speed_X = ls1px->vx;
    Speed_Y = ls1px->vy;
    err = _find(laserx, (elsPos_t)ls1px->ebuf, ls1px->dmin, ls1px->dmax, ls1px->rg, ls1px->sd, ls1px->vy);
    if(err!=lsErr_OK)  return  err;
    else    p = lsTask[ls1px->ebuf].mid;
    
    msg("x:%d, y:%d, dis:%d", (s32)p->pt.x, (s32)p->pt.y, (s32)p->dis);
    {
        point_t goal_pt;
        double  err_a  = 0;                 /* unit : radian */
        double  aim_a  = 0;
        double  l_disx = 0;                 /* local delta distance - x_direction   */
        double  l_disy = 0;                 /* local delta distance - x_direction   */
        gps_t dpt;
        err_a = ls1px->phi;
        
        msg("err_a(degree):%f", err_a*180/3.1415926);
        goal_pt = p->pt;
        
        msg("goalx:%f, goaly:%f", goal_pt.x, goal_pt.y);
        msg("gpsx:%f, gpsy:%f", GPS.position.x, GPS.position.y);
         
        l_disx = (p->dis*cos(err_a) - ls1px->stdx) - sin(err_a)*430;
        l_disy = ls1px->stdy + p->dis*sin(err_a) + ls1px->dia_bar *err_a + 100*sin(err_a);
        aim_a = GPS.radian - err_a;
        
        msg("aim_a(degree):%f", aim_a*180/3.1415926);
        goal_pt.x +=  l_disx*cos(aim_a) - l_disy*sin(aim_a);
        goal_pt.y +=  l_disx*sin(aim_a) + l_disy*cos(aim_a);
        msg("dx:%f dy:%f", goal_pt.x-GPS.position.x, goal_pt.y-GPS.position.y);
        
        _set_keep(goal_pt, aim_a, 600, 150, 10);
        Delay_ms(200);
        _set_keep(goal_pt, aim_a, 600, 150, 3);
        
        SPEED_STOP;
        anyKey(0, "move to goal position");
        
        dpt.position.x = GPS.position.x - goal_pt.x + ls1px->stdx;
        dpt.position.y = GPS.position.y - goal_pt.y + ls1px->stdy;
        dpt.radian     = GPS.radian     - aim_a;
        {
            point_t pt1, pt2;
            pt2 = dpt.position;
            pt1.x = pt2.x*cos(err_a) - pt2.y*sin(err_a);
            pt1.y = pt2.x*sin(err_a) + pt2.y*cos(err_a);
            dpt.position = pt1;
        }
        
        anyKey(0, "dx:%d,dy:%d,da:%f", (s32)(dpt.position.x), (s32)(dpt.position.y), dpt.radian);
        if(anyKey(0," change aim point? press OK")==keyOK)
        {
            ls1px->stdx = dpt.position.x;
            ls1px->stdy = dpt.position.y;
            ls1px->phi  = dpt.radian;
        }
        else
            anyKey(300, "not change");
    }
    return lsErr_OK;
}


/**@sys_lswg
  +----------------------------------------------+
  +----------------------------------------------*/
void sys_lswg(void) /* laser walkground */
{
    static rt_uint32_t key_value;
    
    while( 1 )
    {
        LCD_Clear();
        LCD_SetXY(0, 0);    LCD_WriteString("= laser walkground =");
		LCD_SetXY(0, 1);    LCD_WriteString("1.ld1p");
		LCD_SetXY(0, 2);    LCD_WriteString("2.");
		LCD_SetXY(0, 3);    LCD_WriteString("3.pw1p");
        if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
        {
            switch(key_value)
            {
                case key1: ls1pwg(e1p_ld);    break;
                case key2: ;                  break;
                case key3: ls1pwg(e1p_pw);    break;
                case keyback:                 return;
            }
        }
        Delay_ms(5);
    }
}
