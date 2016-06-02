/*
 * File      : SD_srvice.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2014 XJTU Robot Team
 *
 * Change Logs:
 * Date           Author       Notes
 * 2014-02-16     dong      first implementation
 */
/* ========= includes ========== */
#include "includes.h"
#include <stdlib.h>
#include <string.h>
#include "finsh.h"
#include <stdarg.h>

/* ========= 宏定义 =========== */
/*  在databack文件中存储的最大数据  */
#define  MAX_STORE  5000
/*  在init 文件中存储的最大数据 */
#define  SD_SAVE_LEN  512


/* ======== private ===========*/
static	FATFS 		fs;
static 	FIL  		FileInit;
static	FIL 		FileDataBack;
static 	UINT 		BW;
static 	DWORD		SDRWNum   ; //读写的个数
static 	uint8_t 	SDBuffer[SD_SAVE_LEN];


/* ========== extern ========= */

struct _SDData SDSave[]  = 	
{
/*	name           	  		pointer of value			type     */
	{ "Gyro_convert1"		, &Gyro.convert1 			, Type_Fp64 	} ,
	{ "Gyro_convert2" 	 	, &Gyro.convert2  			, Type_Fp64 	} ,
	{ "Encoder1_convert1"   , &Encoder[0].convert1  	, Type_Fp64 	} ,
	{ "Encoder1_convert2" 	, &Encoder[0].convert2  	, Type_Fp64 	} ,
	{ "Encoder1_r"			, &Encoder[0].r				, Type_Fp64 	} ,
	{ "Encoder1_radian"  	, &Encoder[0].radian 		, Type_Fp64 	} ,
	{ "Encoder2_convert1" 	, &Encoder[0].convert1		, Type_Fp64 	} ,
	{ "Encoder2_convert2"	, &Encoder[0].convert2		, Type_Fp64		} ,
	{ "Encoder2_r"			, &Encoder[0].r				, Type_Fp64		} ,
	{ "Encoder2_radian"		, &Encoder[0].radian		, Type_Fp64		} ,
	{ "d"  	, &Encoder[0].radian 		, Type_Fp64 	} ,
	{ "Encoder2_convert1" 	, &Encoder[0].convert1		, Type_Fp64 	} ,
	{ "Encoder2_convert2"	, &Encoder[0].convert2		, Type_Fp64		} ,
	{ "Encoder2_r"			, &Encoder[0].r				, Type_Fp64		} ,
	{ "Encoder2_radian"		, &Encoder[0].radian		, Type_Fp64		} ,
};

extern int vsprintf(char *string, char *format, va_list param);
/*******************************************************************************
* Function Name  : SD_Write
* Description    : 写字符串到文件中
* Input          : 
* Return         : err
*******************************************************************************/
static FRESULT SD_Write(FIL* fp, UINT* bw, char *fmt,...)
{
	int num;
	char __SD_Printf_Buf[128];
	unsigned char *pStr = (unsigned char *)__SD_Printf_Buf;
	FRESULT res;
	va_list ap;
	
	va_start(ap,fmt);
	num = vsprintf(__SD_Printf_Buf,fmt,ap);	
	va_end(ap);

	res = f_write (fp, pStr, num, bw);
	return res;
}



/*******************************************************************************
* Function Name  : Switch_Save
* Description    : 根据要存储的数据类型选择保存的字节数
*				 : 保存的格式是  name:原始占用的内存:转换成ASCII码后的字符串，方便用户看
*				 : 实际程序读取的是原始占用的内存，所以就算在SD卡中改变字符串的值，也没用。
* Input          : @fp     文件的指针
*				   @sddata 需要储存的数据的结构体 是一个全局变量
* Return         : none
*******************************************************************************/
static void Switch_Save(FIL * fp ,  struct _SDData *sddata  )
{
		static uint32_t BW;
		union udouble udouble_temp 	;
		union uint64  uint64_temp 	;
		union ufloat  ufloat_temp 	;    
		union uint32  uint32_temp 	;
		union uuint32 uuint32_temp	;
		union uint16  uint16_temp	;
		union uuint16 uuint16_temp	;
		//写变量的名字
		SD_Write( fp ,  &BW , "@%s:",sddata->name );
		switch(sddata->value_type){
			case Type_Fp64 :
				udouble_temp.value = *(double*)sddata->value ;
				/* 存储内存中的原始值 */
				f_write( fp , udouble_temp.data , 8 , &BW ) ;
				/* 以字符串的形式打印出来 ，以下类似 */
				SD_Write( fp ,  &BW , ":%lf;    \n", udouble_temp.value);
			break ;
			case Type_Int64 :
				uint64_temp.value = *(int64_t*)sddata->value ;
				f_write( fp , uint64_temp.data , 8 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%lld;    \n", uint64_temp.value);
			break ;
			case Type_Fp32 :
				ufloat_temp.value = *(float*)sddata->value ;
				f_write( fp , ufloat_temp.data , 4 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%f;     \n", ufloat_temp.value);
			break ;
			case Type_Int32 :
				uint32_temp.value = *(int32_t*)sddata->value ;
				f_write( fp , uint32_temp.data , 4 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%d;     \n", uint32_temp.value);
			break ; 
			case Type_Uint32:
				uuint32_temp.value = *(uint32_t*)sddata->value ;
				f_write( fp , uuint32_temp.data , 4 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%u;     \n", uuint32_temp.value);
			break ;
			case Type_Uint16:
				uuint16_temp.value = *(uint16_t*)sddata->value ;
				f_write( fp , uuint16_temp.data , 2 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%d;     \n", uuint16_temp.value);
			break ;
			case Type_Int16 :
				uint16_temp.value = *(int16_t*)sddata->value ;
				f_write( fp , uint16_temp.data , 2 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%d;     \n", uint16_temp.value);
			break ; 				
			case Type_Int8 :
				f_write( fp , (int8_t*)sddata->value  , 1 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%c;     \n", (int8_t*)sddata->value);
			break ; 
			case Type_Uint8 :
				f_write( fp , (uint8_t*)sddata->value , 1 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%c;     \n", (uint8_t*)sddata->value);
			break ; 
			case Type_Gyro :
				
				f_write( fp , (uint8_t*)sddata->value , 1 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%c;     \n", (uint8_t*)sddata->value);
			break ; 
			case Type_Encoder :
				
				f_write( fp , (uint8_t*)sddata->value , 1 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%c;     \n", (uint8_t*)sddata->value);
			break ; 
			case Type_PID :
				
				f_write( fp , (uint8_t*)sddata->value , 1 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%c;     \n", (uint8_t*)sddata->value);
			break ; 
			case Type_Mecanum :
				
				f_write( fp , (uint8_t*)sddata->value , 1 , &BW ) ; 
				SD_Write( fp ,  &BW , ":%c;     \n", (uint8_t*)sddata->value);
			break ;
		}
		f_sync(fp);
}


/*******************************************************************************
* Function Name  : located
* Description    : 定位某个变量的位置
* Input          : 	@str    从SD卡读出来的字符串
*					@sddata 需要储存的数据的结构体 是一个全局变量
* Return         :  没有找到：-1 ， 正确：大于等于0的数
*******************************************************************************/
extern int sscanf( uint8_t *buffer, const char *format, ... );
static int32_t  located(uint8_t *str , struct _SDData *sddata )
{
		int32_t index ;
		char name_str[20]  ;

		for( index = 0 ; index < SD_SAVE_LEN ; index++){
			if(*str == '@'){
				sscanf( str , "%*c%[^:]" , name_str );
				if(sizeof(name_str ) == sizeof(sddata->name) ){
					if(rt_strncmp(name_str , sddata->name ,sizeof(name_str)) == 0){						
						return index ;
					}
				}
			}
			str ++ ;
		}
		return -1 ;
}




/*===========================以下为外部调用的接口函数========================*/

/*******************************************************************************
* Function Name  : ValueInit
* Description    : 初始化某个数据
* Input          : 需要储存的数据的结构体
* Return         : 
*******************************************************************************/
rt_err_t ValueInit( struct _SDData *sddata ) 
{
		FIL *fp = &FileInit ;
		FRESULT res;
		int32_t   location;	
		char * str_ptr = &(sddata->name[0]);
		union udouble udouble_temp 	;
		union uint64  uint64_temp 	;
		union ufloat  ufloat_temp 	;    
		union uint32  uint32_temp 	;
		union uuint32 uuint32_temp	;
		union uint16  uint16_temp		;
		union uuint16 uuint16_temp	;
	
		rt_memset( SDBuffer , 0 ,  SD_SAVE_LEN);
		
		res = f_open (fp, "Init.txt",  FA_READ | FA_WRITE);
		if(res != FR_OK){
				//show message
		}
		
		f_read (fp , SDBuffer , SD_SAVE_LEN , (UINT*)&SDRWNum);

		location = located(SDBuffer , sddata  );
		if(location != -1){		
			switch(sddata->value_type){
				case Type_Fp64 :
						udouble_temp.value  = 0;
						rt_memcpy( udouble_temp.data ,&SDBuffer[location + 2 + strlen(str_ptr)] , 8);
						*(double *)sddata->value = udouble_temp.value ;
					break;
				case Type_Int64 :
						uint64_temp.value = 0 ;
						rt_memcpy( uint64_temp.data ,&SDBuffer[location + 2 + strlen(str_ptr)] , 8);
						*(int64_t *)sddata->value = uint64_temp.value ;
					break;
				case Type_Fp32 :
					ufloat_temp.value = 0;
					  rt_memcpy(ufloat_temp.data , &SDBuffer[location + 2 + strlen(str_ptr)] , 4);
						*(float *)sddata->value = ufloat_temp.value ;
					break ;
				case Type_Int32 :
					uint32_temp.value = 0;
					rt_memcpy( uint32_temp.data ,&SDBuffer[location + 2 + strlen(str_ptr)] , 4);
						*(int32_t *)sddata->value = uint32_temp.value ;
					break;
				case Type_Uint32 :
					uuint32_temp.value = 0;
					  rt_memcpy( uuint32_temp.data , &SDBuffer[location + 2 + strlen(str_ptr)] ,4);
						*( uint32_t*)sddata->value = uuint32_temp.value ;
					break;
				case Type_Int16 :
					  rt_memcpy( uint16_temp.data , &SDBuffer[location + 2 + strlen(str_ptr)] ,2);
						*( int16_t*)sddata->value = uint16_temp.value ;
					break;
				case Type_Uint16 :
					  rt_memcpy(uuint16_temp.data , &SDBuffer[location  + 2 + strlen(str_ptr)] , 2);
						*( uint16_t*)sddata->value = uuint16_temp.value ;
					break;
				case Type_Uint8 :
					  rt_memcpy( ( uint8_t *)sddata->value , &SDBuffer[location + 2 + strlen(str_ptr)] ,1);
					break;
				case Type_Int8 :
					  rt_memcpy( ( int8_t *)sddata->value , &SDBuffer[location  + 2 + strlen(str_ptr)] ,1);
					break;
				case Type_Gyro :
					
					break ; 
				case Type_Encoder :
					
					break ; 
				case Type_PID :
					
					break ; 
				case Type_Mecanum :
					
					break ;
				default :
					break;
			}										
		}
		f_close(fp); 
		return res;		
}


/*******************************************************************************
* Function Name  : ValueSave
* Description    : 
* Input          : @sddata 需要储存的数据的结构体 是一个全局变量
* Return         : none
*******************************************************************************/
int32_t ValueSave( struct _SDData * sddata )
{
		FIL *fp = &FileInit ;
		FRESULT res;
		int32_t location ;

		rt_memset( SDBuffer , 0 ,  SD_SAVE_LEN);
		
		res = f_open (fp, "Init.txt",  FA_READ | FA_WRITE);
		if(res != FR_OK){
			//show message
		}
		/* 先把所有的数据读出来，存储在SDBuffer中 */
		f_read(fp , SDBuffer , SD_SAVE_LEN , &BW );
		/* 定位某个变量的初始位置 */
		location =  located(SDBuffer ,sddata ) ;
		if( location != -1){
			/* 把指针跳转到定位的位置 */
			f_lseek(fp , (DWORD)location);
			/* 存储好数据 */
			Switch_Save( fp , sddata);
		}
		f_close(fp); 
		return res;
}


/*******************************************************************************
* Function Name  : InitAllFromSD
* Description    : 初始化所有的数据
* Input          : none
* Return         : none
*******************************************************************************/
rt_err_t InitAllFromSD(void)
{
	int32_t i ;		
	for(i = 0 ; i< (sizeof(SDSave) - sizeof(SDSave[0]) )/sizeof(SDSave[0]) + 1 ; i++ ){
		ValueInit(&SDSave[i]);
	}
    return 1;
}


/*******************************************************************************
* Function Name  : InitNewCard
* Description    : 如果不小心误删了初始化的文件,可以调用这个函数新建Init.txt文件
* Input          : none
* Return         : none
*******************************************************************************/
void InitNewCard(void)
{
		FIL * fp = &FileInit ;
		int32_t  count ;
		FRESULT res;
		res = f_open (fp, "Init.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
		if(res != FR_OK){
				//show message
		}
		for( count = 0 ; count <  (sizeof(SDSave) - sizeof(SDSave[0]) )/sizeof(SDSave[0]) +1;count++){
			/* 新建的时候给每个变量分配40个字节的空间 */
			f_lseek(fp , count*50);
			Switch_Save(fp , &SDSave[count]) ;
		}
		f_close(fp);
}


/*******************************************************************************
* Function Name  : FatFsInit
* Description    : 文件系统初始化
* Input          : none
* Return         : none
*******************************************************************************/
void FatFsInit(void)
{
		FRESULT 	res;
		SD_Error 	_status ;
		_status 	= SD_Init();
		if(_status != SD_OK){
			//show messagesd : card init error !
			rt_kprintf("card init error !");
		}
		/* 挂载文件系统  */
		res = f_mount(&fs ,"", 0);		
		if(res != FR_OK){
			//show message : FS mount error !
			rt_kprintf("FS mount error !");
		}			
		/* 打开databack文件 */
		res = f_open (&FileDataBack, "DataBack.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
		if(res != FR_OK){
			//show message 
			rt_kprintf("databack open error!");
		}		
}

/*******************************************************************************
* Function Name  : DataStore
* Description    : 把中间数据存储在SD卡中
* Input          : 
* Return         : none
*******************************************************************************/
FRESULT DataStore(char *fmt,...)
{
	uint32_t num , bw;
	FIL *fp = &FileDataBack;
	FRESULT res;
	char __SD_Printf_Buf[128];
	va_list ap;
	unsigned char *pStr = (unsigned char *)__SD_Printf_Buf;
	static int32_t count ;
	
	if( count < MAX_STORE ){
			count ++ ;
			va_start(ap,fmt);
			num = vsprintf(__SD_Printf_Buf,fmt,ap);	
			va_end(ap);
			res = f_write (fp, pStr, num, &bw);
			//f_sync(fp);
			//如果到了最大存储数据量，则把文件关闭
			if(count == MAX_STORE -1){
					f_close(fp);
			}
	}
	return res;
}


/**********************************end of file*************************************/

