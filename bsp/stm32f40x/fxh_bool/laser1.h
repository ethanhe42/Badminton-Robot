
#ifndef __LASER1_H_
#define __LASER1_H_
#include "fxh_bool.h"
#include "mytypedef.h"
typedef struct Point point_t;
typedef struct GPS_State gps_t;

typedef struct
{
    point_t pt;
    double  a;
    double  dis;
}lsPos_t;   /* laser-positon type */

typedef struct
{
    lsPos_t *hd;    /* head */
    lsPos_t *mid;
    int      num;
}lsTask_t;  /* laser task type */

typedef enum
{
    els_pw1,    /* enum laser polewalk */ 
    els_ld1,    /* enum laser ladder1  */
    els_n,      /* enum laser number   */  
}elsPos_t;      /* enum laser position type */

typedef enum
{
    lsErr_OK       = 0x00,  /* correctly */
    lsErr_param    = 0x01,  /* parameter error */
    lsErr_bufof    = 0x02,  /* buffer overflow */
    lsErr_outRange = 0x04,  /* robot go to far */
    lsErr_file     = 0x08,  /* file can't open, probably no sd card */
    lsErr_nfind    = 0x10,  /* can't find the obstacle*/
    lsErr_laser    = 0x20,  /* laser error , probably didnt't power on */
}eLsErr_t;  /* enum laser err type */


/**-----------------------------------------+
  | @data structure for 1 point task        |
  +-----------------------------------------*/
typedef struct
{
    double rg;      /* std_range */
    double phi;     /* phi */
    double stdx;    
    double stdy;
    double dmin;    /* minimum distance */
    double dmax;    /* maximum distance */
    double vx;      /* speed x */
    double vy;      /* speed y */
    elsPos_t  ebuf;   
    int    sd;
    double dia_bar; /* diameter */
}ls1p_t;

typedef enum
{
    e1p_pw,
    e1p_ld,
}els1p_t;





eLsErr_t lss(void);
eLsErr_t ls1p( els1p_t _1px);

void sys_lssp(void); /*laser single point*/
void sys_lswg(void); /* laser walkground */

#endif
