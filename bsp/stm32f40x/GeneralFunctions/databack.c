
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include <math.h>

#if DataBackEnable
/*********跟函数相关的命名为DataBack_Xxx，与数据相关命名为BackDataXxx。*********/

#define BackDataNum 15

static int BackDataCount = 0;
double BackData[3000][BackDataNum];

/*
 * 函数名: DataBack_Write
 * 描  述: 将数据转化为具有四位有效数字的整数，数量级由指数形式给出。如520.1314，发送5201和-1
 * 输  入: 要发送的数据
 * 输  出: 无
 * 调  用: 内、外部调用
 */
void DataBack_Write(double data)
{
	union u16_u8
	{
		int16_t value;
		uint8_t data[2];
	}temp;
	
	uint8_t data_pow = (uint8_t)log10(fabs(data)) - 3;
	temp.value = (int16_t)(data/pow(10.0,data_pow));	//取四位有效数字
	
	Com_SendByte(2,temp.data[0]);
	Com_SendByte(2,temp.data[1]);
	Com_SendByte(2,data_pow);
}

/*
 * 函数名: DataBack_Send
 * 描  述: 发送数据
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void DataBack_Send()
{
	int i,j;
	for(i=0;i<BackDataCount;i++)
		for(j=0;j<BackDataNum;j++)
		{
			DataBack_Write(BackData[i][j]);
			Delay_ms(10);
		}
}

/*
 * 函数名: DataBack_Init
 * 描  述: 数据回传初始化
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void DataBack_Init()
{
	BackDataCount = 0;
}

/*
 * 函数名: DataBack_Save
 * 描  述: 保存数据
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void DataBack_Save()
{
	if(BackDataCount <= 30000)
	{
		BackData[BackDataCount][0]  = 0;
		BackData[BackDataCount][1]  = 0;
		BackData[BackDataCount][2]  = 0;
		BackData[BackDataCount][3]  = 0;
		BackData[BackDataCount][4]  = 0;
		BackData[BackDataCount][5]  = 0;
		BackData[BackDataCount][6]  = 0;
		BackData[BackDataCount][7]  = 0;
		BackData[BackDataCount][8]  = 0;
		BackData[BackDataCount][9]  = 0;
		BackData[BackDataCount][10] = 0;
		BackData[BackDataCount][11] = 0;
		BackData[BackDataCount][12] = 0;
		BackData[BackDataCount][13] = 0;
		BackData[BackDataCount][14] = 0;
		
		BackDataCount++;		
	}
}

/*
 * 函数名: DataBack_GetCount
 * 描  述: 返回一共记录了多少组数据
 * 输  入: 无
 * 输  出: 一共记录了多少组数据
 * 调  用: 外部调用
 */
int DataBack_GetCount()
{
	return BackDataCount;
}

#endif

/**********************************END OF FILE*********************************/
