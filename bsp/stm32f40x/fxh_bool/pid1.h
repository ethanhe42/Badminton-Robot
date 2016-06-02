
#ifndef __PID1_H_
#define __PID1_H_

#include "pid.h"

extern struct PID sPos_Pid[];


typedef enum
{
    eF_sonic,   /* enum front supersonic        */
    eS_sonic,   /* enum front supersonic        */
    eD_LB,      /* enum distance lightboard     */
    eA_LB,      /* enum angle lightboard        */
    eA_Keep,    /* enum angle keep              */
    eD_Keep,    /* enum distance keep           */
    n_pid,      /* number of postion pid list   */
}ePosPID_t;     /* enum position PID type       */

typedef struct PID pid_t;
extern double Pos_Pid(double err, double win, ePosPID_t dir, double max);
extern void _input_pid(pid_t *pid);
#endif
