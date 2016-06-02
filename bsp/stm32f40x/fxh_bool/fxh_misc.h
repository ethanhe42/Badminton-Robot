
#ifndef __FXH_MISC_H_
#define __FXH_MISC_H_

#include "includes.h"

extern int sprintf( char *buffer, const char *format, ...);

typedef struct Point point_t;

extern u8 msg(const char *fmt,... );
u8 anyKey(u32 delay_ms, const char *fmt,... );
extern void _set_keep(
        point_t pt,         /* reference point  */
        double  aim_rad,    /* aim angle        */
        double  maxv,       /* maximum   speed  */
        double  maxa,       /* maximum   speed  */
        double  err         /* acceptable err   */
    );
    
void sys_keep(void);
void sys_debug(void);
void sys_misc(void);
void buz(void);
char *f_namecrt(const char *f_name);
char *f_nameDeal(char *p_char);
#endif
