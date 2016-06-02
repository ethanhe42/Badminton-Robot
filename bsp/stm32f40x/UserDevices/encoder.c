
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

struct Encoder_State Encoder[2];

/*
 * 函数名: Encoder_Init
 * 描  述: 码盘标定系数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Encoder_Init_Con(void)
{
	rt_uint32_t key_value;
	
	LCD_Clear();
	LCD_SetXY(9,0);
	LCD_WriteString("Y+");
	LCD_SetXY(0,1);
	LCD_WriteString("Find a refrence line and press OK to mark the start position");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	MOSI_CMD = ENC_L_INIT;
	Write_Database(M_G_CMD_ID);
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("Y+:Push 3m");
		LCD_SetXY(0,1);
		LCD_WriteString("Press OK to end");
		LCD_SetXY(0,2);
		LCD_WriteDouble(Encoder[0].dis);
		LCD_SetXY(0,3);
		LCD_WriteDouble(Encoder[1].dis);
		Delay_ms(5);
	}
	
	Write_Database(M_G_CMD_ID);
	
	LCD_Clear();
	LCD_SetXY(9,0);
	LCD_WriteString("Y-");
	LCD_SetXY(0,1);
	LCD_WriteString("Find a refrence line and press OK to mark the start position");
	
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	Write_Database(M_G_CMD_ID);
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("Y-:Push 3m");
		LCD_SetXY(0,1);
		LCD_WriteString("Press OK to end");
		LCD_SetXY(0,2);
		LCD_WriteDouble(Encoder[0].dis);
		LCD_SetXY(0,3);
		LCD_WriteDouble(Encoder[1].dis);
		Delay_ms(5);
	}
	
	Write_Database(M_G_CMD_ID);
	
	LCD_Clear();
	LCD_SetXY(9,0);
	LCD_WriteString("X+");
	LCD_SetXY(0,1);
	LCD_WriteString("Find a refrence line and press OK to mark the start position");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	Write_Database(M_G_CMD_ID);
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("X+:Push 3m");
		LCD_SetXY(0,1);
		LCD_WriteString("Press OK to end");
		LCD_SetXY(0,2);
		LCD_WriteDouble(Encoder[0].dis);
		LCD_SetXY(0,3);
		LCD_WriteDouble(Encoder[1].dis);
		Delay_ms(5);
	}
	
	Write_Database(M_G_CMD_ID);
	
	LCD_Clear();
	LCD_SetXY(9,0);
	LCD_WriteString("X-");
	LCD_SetXY(0,1);
	LCD_WriteString("Find a refrence line and press OK to mark the start position");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	Write_Database(M_G_CMD_ID);
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("X-:Push 3m");
		LCD_SetXY(0,1);
		LCD_WriteString("Press OK to end");
		LCD_SetXY(0,2);
		LCD_WriteDouble(Encoder[0].dis);
		LCD_SetXY(0,3);
		LCD_WriteDouble(Encoder[1].dis);
		Delay_ms(5);
	}
	
	Write_Database(M_G_CMD_ID);
	
	Delay_ms(10);
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("C11:");
	LCD_WriteDouble(Encoder[0].convert1);
	LCD_SetXY(0,1);
	LCD_WriteString("C12:");
	LCD_WriteDouble(Encoder[0].convert2);
	LCD_SetXY(0,2);
	LCD_WriteString("C21:");
	LCD_WriteDouble(Encoder[1].convert1);
	LCD_SetXY(0,3);
	LCD_WriteString("C22:");
	LCD_WriteDouble(Encoder[1].convert2);
	LCD_SetXY(0,4);
	LCD_WriteString("A1:");
	LCD_WriteDouble(Encoder[0].radian);
	LCD_SetXY(0,5);
	LCD_WriteString("A2:");
	LCD_WriteDouble(Encoder[1].radian);
	LCD_SetXY(0,6);
	LCD_WriteString("Press any key to continue...");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
}

/*
 * 函数名: Encoder_Init_R
 * 描  述: 码盘标定旋转半径
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */

void Encoder_Init_R(void)
{
	rt_uint32_t key_value;
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("Find a refrence and press OK to turn 5+ circles.");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
	MOSI_CMD = ENC_R_INIT;
	Write_Database(M_G_CMD_ID);
	
	while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
	{		
		LCD_Clear();
		LCD_SetXY(0,0); 
		LCD_WriteString("turning...");
		LCD_SetXY(0,1);
		LCD_WriteString("Press any key to stop");
		LCD_WriteNum(Gyro.angle);
		
		Speed_Rotation = 50;
		Delay_ms(5);
	}
	
	SPEED_STOP;
	Write_Database(M_G_CMD_ID);
	
	Delay_ms(10);
	
	LCD_Clear();
	LCD_SetXY(0,0);
	LCD_WriteString("R1:");
	LCD_WriteDouble(Encoder[0].r);
	LCD_SetXY(0,1);
	LCD_WriteString("R2:");
	LCD_WriteDouble(Encoder[1].r);
	LCD_WriteString("Press any key to continue...");
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
}

/*
 * 函数名: Encoder_View
 * 描  述: 查看码盘的信息
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Encoder_View(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("C11:");
		LCD_WriteDouble(Encoder[0].convert1);
		LCD_SetXY(0,1);
		LCD_WriteString("C12:");
		LCD_WriteDouble(Encoder[0].convert2);
		LCD_SetXY(0,2);
		LCD_WriteString("C21:");
		LCD_WriteDouble(Encoder[1].convert1);
		LCD_SetXY(0,3);
		LCD_WriteString("C22:");
		LCD_WriteDouble(Encoder[1].convert2);
		LCD_SetXY(0,4);
		LCD_WriteString("A1:");
		LCD_WriteDouble(Encoder[0].radian);
		LCD_SetXY(0,5);
		LCD_WriteString("A2:");
		LCD_WriteDouble(Encoder[1].radian);
		LCD_SetXY(0,6);
		LCD_WriteString("R1:");
		LCD_WriteDouble(Encoder[0].r);
		LCD_SetXY(0,7);
		LCD_WriteString("R2:");
		LCD_WriteDouble(Encoder[1].r);
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: Encoder_Set
 * 描  述: 设置码盘的标定系数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Encoder_Set(void)
{
	MOSI_ENC1_C1.f64_data = MISO_ENC1_C1.f64_data;
	MOSI_ENC1_C2.f64_data = MISO_ENC1_C2.f64_data;
	MOSI_ENC2_C1.f64_data = MISO_ENC2_C1.f64_data;
	MOSI_ENC2_C2.f64_data = MISO_ENC1_C2.f64_data;
	MOSI_ENC1_ANG.f64_data = MISO_ENC1_ANG.f64_data;
	MOSI_ENC2_ANG.f64_data = MISO_ENC2_ANG.f64_data;
	MOSI_ENC1_RAD.f64_data = MISO_ENC1_RAD.f64_data;
	MOSI_ENC2_RAD.f64_data = MISO_ENC2_RAD.f64_data;
	
	
	Input_DoubleValue(&MOSI_ENC1_C1.f64_data,"C11");
	Write_Database(W_ENC1_C1_ID);
	
	Input_DoubleValue(&MOSI_ENC1_C2.f64_data,"C12");
	Write_Database(W_ENC1_C2_ID);
	
	Input_DoubleValue(&MOSI_ENC2_C1.f64_data,"C21");
	Write_Database(W_ENC2_C1_ID);
	
	Input_DoubleValue(&MOSI_ENC2_C2.f64_data,"C22");
	Write_Database(W_ENC2_C2_ID);
	
	Input_DoubleValue(&MOSI_ENC1_ANG.f64_data,"Radian1");
	Write_Database(W_ENC1_ANG_ID);
	
	Input_DoubleValue(&MOSI_ENC2_ANG.f64_data,"Radian2");
	Write_Database(W_ENC2_ANG_ID);
	
	Input_DoubleValue(&MOSI_ENC1_RAD.f64_data,"R1");
	Write_Database(W_ENC1_RAD_ID);
	
	Input_DoubleValue(&MOSI_ENC2_RAD.f64_data,"R2");
	Write_Database(W_ENC2_RAD_ID);
}


/**********************************END OF FILE*********************************/
