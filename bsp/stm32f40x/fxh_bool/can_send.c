
#include "can_send.h"
#include "mytypedef.h"
typedef struct Point point_t;
typedef struct GPS_State gps_t;

typedef struct
{
    char        *t_name;    /* thread name */
    gps_t       gps;        /* save GPS*/
    
}can_send_t;   /* can send infomation */

