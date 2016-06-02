/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYTYPEDEF_H
#define __MYTYPEDEF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//点结构体
struct Point
{
	double x;
	double y;
};

//路径信息结构体
struct Path_Point
{
	struct Point position;	//位置
	double speeddir;		//速度方向
	double selfdir;			//车体方向
	double speed;			//速度大小
};

//陀螺仪信息结构体
struct Gyro_State
{
	double angle;			//角度
	double convert1;		//陀螺仪正转系数
	double convert2;		//陀螺仪反转系数
};
//码盘信息结构体
struct Encoder_State
{
	double dis;				//距离
	double convert1;		//正转系数
	double convert2;		//反转系数
	double r;				//车身自转时码盘的旋转半径
	double radian;			//码盘与车身坐标系Y轴正方向的夹角
};

//全场定位信息结构体
struct GPS_State
{
	struct Point position;	//
	double radian;			//弧度
};

//PID结构体
struct PID
{
	double p;
	double i;
	double d;
	double mend;
	double ep1;
	double ep2;
};

//每个轮子对应的状态
struct Mecanum_State
{
	int port;				//对应的PWM端口
	int arg;				//方向系数
};

//每个轮子对应的状态
struct Swedish_State
{
	int port;				//对应的PWM端口
	int arg;				//方向系数
};

//按键值
enum M_KeyNumTpye
{
    key1 = 1,
    key2,
    key3,
    key4,
    keyback,
    emergency,
    
    key5,
    key6,
    key7,
    key8,
    empty1,
    empty2,
    
    key9,
    key0,
    point,
    sign,
    keydelete,
    keyOK
};
#define pageup point
#define pagedown sign

typedef enum {
 		Type_Fp64 = 1 ,
 		Type_Int64,
 		Type_Fp32 ,
 		Type_Int32,
 		Type_Uint32,
 		Type_Int16,
 		Type_Uint16,
 		Type_Int8,
 		Type_Uint8,
 		Type_Gyro,
 		Type_Encoder,
 		Type_PID,
 		Type_Mecanum,
}SD_Value_Type;


typedef enum {
      TYPE_INT8	=  1,
      TYPE_UINT8	,
      TYPE_INT16	,
      TYPE_UINT16	,
      TYPE_INT32	,
      TYPE_UINT32	,
      TYPE_INT64	,
      TYPE_UINT64	,
      TYPE_FP32		,
      TYPE_FP64		,
//      TYPE_POINTFP32,
//      TYPE_POINTFP64,
}Value_Type;

//列表显示与更改
struct Input_ListValue
{
	void *value;
	Value_Type type;
	char *name;
};

#endif 

/**********************************END OF FILE*********************************/
