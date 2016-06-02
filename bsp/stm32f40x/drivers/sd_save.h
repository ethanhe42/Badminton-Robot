
#ifndef __SD_SAVE_H_
#define __SD_SAVE_H_
#include "stm32f4xx.h"
#include "includes.h"

typedef struct
{
    u32 w_index;
    u32 r_index;
    u8 buf[8*128*6];
}img_cbuf_t;    /* image circular buffer type */


typedef enum
{
    camErr_OK       = 0x00,  
    camErr_Timeout  = 0x01,
    camErr_Param    = 0x02,
    camErr_it       = 0x04, /* err in interrupt handler(can) */
    camErr_bufof    = 0x08, /* buffer overflow */
    camErr_frame    = 0x10,
    camErr_file     = 0x20,
    camErr_can      = 0x40,
}cam_err;

/* camera mode */
typedef enum
{
    camMod_norm,
    camMod_tran,
}cam_mode;

typedef enum
{
    camSta_NoReady,
    camSta_Ready,       /* camra state ready to transmit image */
    camSta_Transmit,
    camSta_Finish,
}cam_state;             /* camera state */

#define IMG_SIZE     (320*240*2)

extern img_cbuf_t Img_Cbuf;
extern int Cam_Sta[7];
extern int Cam_Mode[7];
extern int Cam_ErrFlg[7];
extern u8  cam_trsimg_fh[];
extern FRESULT SD_Printf (FIL* fp, char *fmt,...);
FRESULT SD_Printf_MQ(FIL* fp, char *fmt,...);
extern FRESULT SD_Printf2(FIL* fp, char *fmt,...);
extern FRESULT f_timestamp(char *fname);
extern u8 Save_Img(u8 *f_name, u8 camx);

#endif

