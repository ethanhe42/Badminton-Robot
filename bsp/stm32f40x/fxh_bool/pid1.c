

#include "includes.h"
#include "pid1.h"

struct PID sPos_Pid[] = 
{
    //{ 30.0,  0.003,  0   },  //eF_sonic,   /* enum front supersonic */
    { 0.015,  0.00 ,  0   },  //eF_sonic,   /* enum front supersonic */
    { 1.0,  0.001,  0   },  //eS_sonic,   /* enum front supersonic */
    { 16.0, 0.0,    10.0},  //eD_LB,      /* enum distance lightboard */
    { 10.0,  0.0,    0   },  //eA_LB,      /* enum angle lightboard */
    { 0.85, 0.0,    3   },	//4  KeepΩ«∂»PID
	{ 6,    0.0,    3   },	//5  Keepæ‡¿ÎPID
};

static double pe[n_pid], ie[n_pid], de[n_pid];
static double e0[n_pid], e1[n_pid];//, e2[n_pid]
static double u[n_pid];

void Pos_PidClr(void)
{
    int i;
    
    /* check sPos_Pid[] */
    if(sizeof(sPos_Pid)/sizeof(sPos_Pid[0]) != n_pid)
    {
        LCD_ERROR();
        msg("sPos_Pid size don't match func[%s]", __FUNCTION__);
    }
    
    for(i=0; i<n_pid; i++)
    {
        pe[i]=0; ie[i]=0; de[i]=0;
        e0[i]=0; e1[i]=0;// e2[i]=0;
        u[i] =0;
    }
}

double Pos_Pid(double err, double win, ePosPID_t dir, double max)
{

    struct PID *pid = &sPos_Pid[dir];
    
    e0[dir] = err;
    
    pe[dir] =  e0[dir];
    ie[dir] += e0[dir];
    ie[dir] *= win;
    de[dir] =  e0[dir] - e1[dir];    
    u[dir] = (pid->p*pe[dir] + pid->i*ie[dir] + pid->d*de[dir]);
    
    e1[dir] = e0[dir];
//    e2[dir] = e1[dir];

    /* œﬁ∑˘ */
    if(u[dir]>max)   
        u[dir] = max;
    else if(u[dir]<-max) 
        u[dir] = -max;
    
    return u[dir];//400.0
}

void _input_pid(pid_t *pid)
{
	rt_uint32_t key_value;
    
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0, 0); LCD_WriteString("===input pid===");
		LCD_SetXY(0, 1); LCD_WriteString("1.p");
        LCD_SetXY(0, 2); LCD_WriteString("2.i");
        LCD_SetXY(0, 3); LCD_WriteString("3.d");

		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
                    Input_DoubleValue(&pid->p, "p");
				break;
				case key2 :
                    Input_DoubleValue(&pid->i, "i");
                break;
                    
                case key3 :
                    Input_DoubleValue(&pid->d, "d");
                break;
                
				case keyback:
                return;
			}
		}
		Delay_ms(10);
	}
}


// /**----------------------------------------------------------------*
//  |  angle pid @author : owen . @modified by fxh_bool               |
//  *-----------------------------------------------------------------*/
// static __inline double _angle_pid(double err,pid_t pid) 
// {
// 	double A=0;
// 	double B=0;
// 	double C=0;
// 	double delta=0;
//     static double a_err1=0;
//     static double a_err2=0;
//     static double a_mend=0;

// 	
// 	A = (pid.p*(1+1/pid.i+pid.d));
// 	B = -pid.p*(1+2*pid.d);
// 	C = pid.p*pid.d;
// 	
// 	delta = A*err + B*a_err1 + C*a_err2;
// 	a_mend += delta;
// 	
// 	a_err2=a_err1;
// 	a_err1=err;
// 	
// 	return a_mend;
// }

// /**----------------------------------------------------------------*
//  |  angle pid @author : owen . @modified by fxh_bool               |
//  *-----------------------------------------------------------------*/
// static __inline double _dis_pid(double err, pid_t pid)
// {
// 	double A=0;
// 	double B=0;
// 	double C=0;
// 	double delta=0;
//     static double d_err1=0;
//     static double d_err2=0;
//     static double d_mend=0;
// 	
// 	A = (pid.p*(1+1/pid.i+pid.d));
// 	B = -pid.p*(1+2*pid.d);
// 	C = pid.p*pid.d;
// 	
// 	delta = A*err + B*d_err1 + C*d_err2;
// 	d_mend+=delta;
// 	
// 	d_err2=d_err1;
// 	d_err1=err;
// 	
// 	return d_mend;
// }

