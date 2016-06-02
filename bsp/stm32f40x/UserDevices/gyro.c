
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/*Gyro Convert:-1.452*/

struct Gyro_State Gyro;

/*
 * 函数名: Gyro_Init
 * 描  述: 陀螺仪标定系数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Gyro_Init(void)
{
	rt_uint32_t key_value;
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Find a refrence border and press OK to mark the start angle");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	MOSI_CMD = GYRO_INIT;
	Write_Database(M_G_CMD_ID);
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Press any key to start turning +10 cicles.");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);

	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("turning...");
		LCD_SetXY(0,1);
		LCD_WriteString("Press any key to stop");
		
		Speed_Rotation = 50;
		Delay_ms(5);
	}
	
	SPEED_STOP;
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Back to the refrence border and press OK to end");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	Write_Database(M_G_CMD_ID);
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Press any key to start turning -10 cicles.");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("turning...");
		LCD_SetXY(0,1);
		LCD_WriteString("Press any key to stop");
		
		Speed_Rotation = -50;
		Delay_ms(5);
	}
	
	SPEED_STOP;
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Back to the refrence border and press OK to end");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	Write_Database(M_G_CMD_ID);
	
	Delay_ms(10);
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Con1:");
	LCD_WriteDouble(Gyro.convert1);
	LCD_SetXY(0,1);
	LCD_WriteString("Con2:");
	LCD_WriteDouble(Gyro.convert2);
	LCD_SetXY(0,2);
	LCD_WriteString("Press any key to continue...");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
}

/*
 * 函数名: Gyro_Set
 * 描  述: 设置陀螺仪的标定系数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Gyro_Set(void)
{
	rt_uint32_t key_value;
	
	MOSI_GYRO_C1.f64_data = MISO_GYRO_C1.f64_data;
	MOSI_GYRO_C2.f64_data = MISO_GYRO_C2.f64_data;
	
	Input_DoubleValue(&MOSI_GYRO_C1.f64_data,"Convert1");
	Write_Database(W_GYRO_C1_ID);
	
	Input_DoubleValue(&MOSI_GYRO_C2.f64_data,"Convert2");
	Write_Database(W_GYRO_C2_ID);
	
	Delay_ms(10);
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("New Convert1:");
	LCD_WriteDouble(Gyro.convert1);
	LCD_SetXY(0,1);
	LCD_WriteString("New Convert2:");
	LCD_WriteDouble(Gyro.convert2);
	LCD_SetXY(0,2);
	LCD_WriteString("Press any key to continue...");
	
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	
}


/*
 * 函数名: Gyro_CheckFloat
 * 描  述: 陀螺仪校零漂
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Gyro_CheckFloat(void)
{
	rt_uint32_t key_value;
	int i=0;
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(3,0);
		LCD_WriteString("Check Float~!");
		LCD_SetXY(0,2);
		LCD_WriteString("Don't move the robot");
		LCD_SetXY(8,3);
		LCD_WriteString("!!!!");
		CheckFloat();
		
		while(MISO_CMD == NO_COMMAND)
		{
			LCD_SetXY(i,5);
			LCD_WriteString(">");
			i++;
			Delay_ms(200);
			
			if(i==20)
			{
				i=0;
				LCD_SetXY(0,5);
				LCD_WriteString("                    ");
				Delay_ms(100);
			}
			
			if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
				return;
		}
		
		LCD_Clear();
		LCD_SetXY(0,3);
		
		switch(MISO_CMD)
		{
			case CF_NORMAL:
				LCD_WriteString("Gyro Init Success!!");
				break;
			case CF_CHANGED:
				LCD_WriteString("Zero Float Changed!!");
				break;
			case CF_ERROR:
				LCD_WriteString("Gyro Init Error!!");
				break;
		}
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK)
			return;
	}
}


/**********************************END OF FILE*********************************/
