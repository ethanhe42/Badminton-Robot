
#include "includes.h"
#include "string.h"
ALIGN(RT_ALIGN_SIZE)
static char thread_FATfs_stack[4096];
struct rt_thread thread_FATfs;
extern	SD_Error status;
static FATFS fs;

extern FIL fil_walkground;
extern double StartAngle;
extern struct GPS_State GPS_Record[];

void SD_ReInit(void)
{
	rt_thread_detach(&thread_FATfs);
	rt_thread_FATfs_init();
}
void SD_Walkground(void)
{
//	rt_thread_detach(&thread_FATfs);
//	rt_thread_FATfs_init();
	int i;
	
	f_open (&fil_walkground, "walkground.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	f_lseek(&fil_pid, fil_pid.fsize);
	
	SD_Printf_MQ(&fil_walkground,"float Motor1_Aim_Pos[] = { %f,\t%f,\t%f,\t%f,\t%f,\t%f};\r\n",
		Motor1_Aim_Pos[0],
		Motor1_Aim_Pos[1],
		Motor1_Aim_Pos[2],
		Motor1_Aim_Pos[3],
		Motor1_Aim_Pos[4],
		Motor1_Aim_Pos[5]);
	
	Delay_ms(5);
	
	SD_Printf_MQ(&fil_walkground,"float Motor2_Aim_Pos[] = { %f,\t%f,\t%f,\t%f,\t%f,\t%f};\r\n",
		Motor2_Aim_Pos[0],
		Motor2_Aim_Pos[1],
		Motor2_Aim_Pos[2],
		Motor2_Aim_Pos[3],
		Motor2_Aim_Pos[4],
		Motor2_Aim_Pos[5]);
	
	Delay_ms(5);
	
	SD_Printf_MQ(&fil_walkground,"float Motor3_Aim_Pos[] = %f,%f;\r\n",Motor3_Aim_Pos[0],Motor3_Aim_Pos[1]);
	
	Delay_ms(5);
	
	SD_Printf_MQ(&fil_walkground,"StartAngle:%f\r\n",StartAngle);
	for(i=0;i<7;i++)
	{
		SD_Printf_MQ(&fil_walkground,"%f,%f,%f;\r\n",GPS_Record[i].position.x,GPS_Record[i].position.y,GPS_Record[i].radian);
		Delay_ms(5);
	}
        
	Delay_ms(5);
	SD_Printf_MQ(&fil_walkground,"SetPole(%d,%d,%f);\r\n",
		Aim_X_Pole,
		Aim_Y_Pole,
		Aim_A_Pole);
        
	Delay_ms(5);
	SD_Printf_MQ(&fil_walkground,"SetHandover(%d,%d,%d,%d,Aim_ss);\r\n",
		Aim_X_Handover[0],
		Aim_Y_Handover[0],
		Aim_Z_Handover[0],
		Aim_A_Handover[0]);
		
	Delay_ms(5);
	SD_Printf_MQ(&fil_walkground,"SetHandover(%d,%d,%d,%d,Aim_pw);\r\n",
		Aim_X_Handover[1],
		Aim_Y_Handover[1],
		Aim_Z_Handover[1],
		Aim_A_Handover[1]);
		
	Delay_ms(5);
	if(GROUND==RED)
	{
		SD_Printf_MQ(&fil_walkground,"SetSwing(%f,%f,%f,RED);\r\n",
		Aim_D_Swing[Cam_Circle_Line],
		Aim_D_Swing[GROUND+1],
		Aim_A_Swing[GROUND]);
	}

	if(GROUND==BLUE)
	{
		SD_Printf_MQ(&fil_walkground,"SetSwing(%f,%f,%f,BLUE);\r\n",
		Aim_D_Swing[Cam_Circle_Line],
		Aim_D_Swing[GROUND+1],
		Aim_A_Swing[GROUND]);			
	}

	SD_Printf_MQ(&fil_walkground,"THE END");
	
    LCD_Clear();
		LCD_SetXY(0,2);
		LCD_Printf("Done");
    Delay_ms(200);
}


/*
 * 函数名: rt_thread_entry_FATfs
 * 描  述: 文件系统线程的入口函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 内部调用
 * 说  明: 
 */
//  FRESULT f_stat (
//   const TCHAR* path,  /* [IN] Object name */
//   FILINFO* fno        /* [OUT] FILINFO structure */
// );
FIL fil_pid;
FIL Mortor_info;
extern void cam_filcrt(void);   /* camra file create */

static void Mortor_filcrt(void)
{

    FRESULT res;
    
    res = f_open (&Mortor_info, "Mortor_info.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);    //FA_OPEN_ALWAYS就是如果文件不存在，就创建
    //res = f_lseek(&fil_pid, fil_pid.fsize);   // 接着往下写，不是从头写
    //res = f_printf(&fil_pid, "f.txt");
    res = f_close(&Mortor_info);                   //只有有了关闭操作，文件才会完整
    res = f_timestamp("Mortor_info.txt");            //给文件打上时间戳（在文件关闭的前提下）
	res = f_open (&Mortor_info, "Mortor_info.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    if(res!=FR_OK)
		SD_RES(res);
}


static void route_filcrt(void)
{

    FRESULT res;
    
    res = f_open (&fil_pid, "f_pid.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);    //FA_OPEN_ALWAYS就是如果文件不存在，就创建
    //res = f_lseek(&fil_pid, fil_pid.fsize);   // 接着往下写，不是从头写
    //res = f_printf(&fil_pid, "f.txt");
    res = f_close(&fil_pid);                   //只有有了关闭操作，文件才会完整
    res = f_timestamp("f_pid.txt");            //给文件打上时间戳（在文件关闭的前提下）
	res = f_open (&fil_pid, "f_pid.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);    //FA_OPEN_ALWAYS就是如果文件不存在，就创建
    SD_RES(res);
}

static void set_dir(void)
{
    FRESULT res;
    FILINFO fno;
    int i=0;
    DIR dir;
    static char buf[50];
    sprintf(buf, "%s_%2d", __DATE__, 3);
    f_nameDeal(buf);
    
    while(1)
    {
        sprintf(buf, "%s_%3d", __DATE__, i);
        f_nameDeal(buf);
        res = f_stat(buf, &fno);
        if (res!=FR_OK)
            break;
        i++;
    }
    sprintf(buf, "%s_%3d", __DATE__, i);
    f_nameDeal(buf);
    res = f_mkdir((const char *)buf);
    res = f_opendir(&dir, (const char *)buf);
    res = f_closedir(&dir);
    res = f_timestamp((char *)buf);
    res = f_chdir((const char *)buf);
}
 
static void rt_thread_entry_FATfs(void* parameter)
{
	char buf[128];
	uint32_t write_count = 0;
	FIL *file_tmp;
	uint32_t BytesWritten = 0;
	union {
		uint32_t temp;
		char     arr[4];
	}trs;
		
    FRESULT res;

    Delay_ms(50);
    status = SD_Init();
    if(status != SD_OK)
    {
        rt_kprintf("sd card init error.\n\r");
    }
    
    res = f_mount(&fs ,"", 0);
    SD_RES(res);
    rt_kprintf(" Capacity :%d MB\r\n", SDCardInfo.CardCapacity>>20);
    
    /** @attention : 以日期时间作为文件名 */
    set_dir();
	
	//返回数据
	route_filcrt();
    
    //创建摄像头文件
    cam_filcrt();
	
    while(1)
	{  
//        rt_thread_delay(2);
		/*
		*
		*		请不要在该线程中添加代码！！！
		*
		*/
		rt_memset(buf, 0, sizeof(buf));
		if(rt_mq_recv(&mq_sd, &buf[0], sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
		{
			/*获得文件指针*/
			trs.arr[0] = buf[0];
			trs.arr[1] = buf[1];
			trs.arr[2] = buf[2];
			trs.arr[3] = buf[3];
			file_tmp = (FIL *)(trs.temp);
			/*移动到上次所写字节之后*/
//			f_lseek(file_tmp, file_tmp->fptr);
			/*获得文件需写字节数*/
			trs.arr[0] = buf[4];
			trs.arr[1] = buf[5];
			trs.arr[2] = buf[6];
			trs.arr[3] = buf[7];
			write_count = trs.temp;
			
			f_write(file_tmp, &buf[8], write_count, &BytesWritten);
            if(!strcmp(&buf[8], "THE END"))
            {
                rt_thread_delay(1);
                f_close(file_tmp);
            }
		}
    }	
}                      

void sd_reInit(void)
{
		
    FRESULT res;

    Delay_ms(50);
    status = SD_Init();
    if(status != SD_OK)
    {
        rt_kprintf("sd card init error.\n\r");
    }
    
    res = f_mount(&fs ,"", 0);
    rt_kprintf(" Capacity :%d MB\r\n", SDCardInfo.CardCapacity>>20);
    
    /** @attention : 以日期时间作为文件名 */
    set_dir();
	
	//返回数据
	route_filcrt();
    
    //创建摄像头文件
    cam_filcrt();
	
	Mortor_filcrt();
	
    res = f_open (&fil_pid, "f_pid.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);    //FA_OPEN_ALWAYS就是如果文件不存在，就创建

    if(res) ((void) 0);
}


/*
 * 函数名: rt_thread_FATfs_init
 * 描  述: 文件系统线程的初始化
 * 输  入: 无
 * 输  出: 初始化结果
 * 调  用: 外部调用
 */
int rt_thread_FATfs_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_FATfs,
						 "FatFs",
						 rt_thread_entry_FATfs,//
						 RT_NULL,
						 &thread_FATfs_stack[0],
						 4096,RT_THREAD_PRIORITY_MAX - 5,5);
    
    if (result == RT_EOK) 
        rt_thread_startup( &thread_FATfs );
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
