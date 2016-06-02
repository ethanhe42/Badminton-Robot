#ifndef _INCLUDES_H
#define _INCLUDES_H

//#define NOPID

#include <rthw.h>
#include <rtthread.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/* Application */
#include "mb.h"
#include "mq.h"

/* thread */
#include "t_handle.h"
#include "Y_handle.h"
#include "t_fatfs.h"
#include "t_test.h"
#include "t_catching.h"
#include "t_timer.h"
#include "t_catching.h"
#include "t_realspe.h"
/* Menu */

/* UserDevices */
#include "Lcd_Keyboard.h"
#include "Mecanum.h"
#include "Swedish.h"
#include "Gyro.h"
#include "Encoder.h"
#include "GPS.h"
#include "Relay.h"
#include "Motor.h"
#include "Camera.h"
#include "laser.h"
#include "LightBoard.h"
#include "sonic.h"
#include "fxh_bool.h"

/* Route */
#include "GoLine.h"
#include "EasyLine.h"
#include "GoCircle.h"
#include "Keep.h"
#include "PointPath.h"

/*Auto*/
#include "ArmAction.h"
#include "GoRoute.h"

/* GeneralFunction */
#include "MyTypeDef.h"
#include "MyString.h"
#include "MyMath.h"
#include "PID.h"
#include "DataBack.h"

/*SDIO*/
#include "stm324xg_eval_sdio_sd.h"
#include "ff.h"

/* Driver */
#include "board.h"
#include "io.h"
#include "usart.h"
#include "dma.h"
#include "dma_rx_isr.h"
#include "serial.h"
#include "pwm.h"
#include "can.h"
#include "SD_Serve.h"
#include "can_database.h"
#include "Command.h"
#include "fan.h"
#include "sd_save.h"
#include "M_sys.h"

/*------------------------------------------------------------------*/
 #define LCD_DEBUG 1                                                //                    
                                                                    //
 #if LCD_DEBUG                                                      //
 void LCD_Assert_Fail(const char *func, const int line);            //
	 #define LCD_ERROR() LCD_Assert_Fail(__FUNCTION__, __LINE__)    //
#else                                                               //
    #define LCD_ERROR() (void)0;                                    //
 #endif                                                             //
/*------------------------------------------------------------------*/

#define TWOHANDLE

/*--------------------------------------------------------------------------*/
 #define SD_SHOWRES 1                                                       //
                                                                            //
 #if SD_SHOWRES                                                             //
	 #define SD_RES(res) \
                sd_res(res, __FUNCTION__, __LINE__)                         //
 #else                                                                      //
    #define SD_RES(FRESULT res) (void)0                                     //
 #endif                                                                     //
/*--------------------------------------------------------------------------*/
	
/*-------------------------------------------+
 |   红蓝场的宏定义                         |
 +------------------------------------------*/
 #define  BLUE_GROUND    0                  //
 #define  RED_GROUND     1                  //这两个值千万别改！
 #define  BLUE           BLUE_GROUND        //
 #define  RED            RED_GROUND         //
                                            //
 #define  GROUND		 BLUE                //
                                            //
/*------------------------------------------*/

#endif
