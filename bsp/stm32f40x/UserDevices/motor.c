/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/****************************老控制盒相关命令******************************/

//老电机控制盒

u8		IO_State[8];//电机控制盒上的ST的状态，比如:电机1对应 IO_State[0]
float	Motor_Pos[8];//电机的绝对位置，比如:电机1对应 Motor_Pos[0]
int16_t Motor_RealAim[8];//电机控制盒认为的目标位置*1000后得到的整数

void Moto_Stop(ID_NUMDEF ID_NUM)
{
	W_MOTOR_OLD_FUNC(ID_NUM, 0 , 0 , CMD_STOP);
}

void Moto_Set_NPos(ID_NUMDEF ID_NUM , float rot)	//set now position to xxx
{
	W_MOTOR_OLD_FUNC(ID_NUM, rot , 0 , CMD_SET_PN);
}

void Moto_Clear_NPos(ID_NUMDEF ID_NUM)	//
{
	W_MOTOR_OLD_FUNC(ID_NUM, 0 , 0 , CMD_DEFAULT);
}

void Moto_Set_GPos(ID_NUMDEF ID_NUM, float rot )	//goal possition
{
	//W_MOTOR_OLD_FUNC(ID_NUM, rot , 0 , CMD_SET_PG);
	W_MOTOR_OLD_FUNC(ID_NUM, rot , 10000 , CMD_SET_PSG);
}

void Moto_Set_GSpeed(ID_NUMDEF ID_NUM, int16_t speed )   //goal speed
{
	W_MOTOR_OLD_FUNC(ID_NUM, 0 , speed , CMD_SET_SG);
}


/*
 * 函数名: Motor_Test
 * 描  述: 上层电机测试函数
 * 输  入: 电机ID
 * 输  出: 无
 * 调  用: 外部调用
 */
void Motor_Test(ID_NUMDEF motor)
{
	rt_uint32_t key_value;
	double rotation;
	int speed_m;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("=======Motor%d=======",motor-W_MOTOR1_OLD_ID+1);
		LCD_SetXY(0,1);
		LCD_Printf("1.STOP %f",Motor_RealAim[motor-W_MOTOR1_OLD_ID]/1000.0);
		LCD_SetXY(0,2);
		LCD_WriteString("2.Set Position");
		LCD_SetXY(0,3);
		LCD_WriteString("3.Set Speed");
		LCD_SetXY(0,4);
		LCD_WriteString("4.Set Pos and Speed");
		LCD_SetXY(0,5);
		LCD_WriteString("5.Clear N_Pos");
		LCD_SetXY(14,5);
		LCD_WriteString("6.Can");
		LCD_SetXY(0,6);
		LCD_Printf("Now Pos:%g",Motor_Pos[motor-W_MOTOR1_OLD_ID]);
		LCD_SetXY(0,7);
		LCD_Printf("IO Status:%d %d %d %d",M_IO_READ(motor,1),M_IO_READ(motor,2),M_IO_READ(motor,3),M_IO_READ(motor,4));
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Moto_Stop(motor);
					break;
				case key2:
					Input_DoubleValue(&rotation,"Goal Position");
					Moto_Set_GPos(motor,(float)rotation);
					break;
				case key3:
					Input_IntValue(&speed_m,"Goal Speed");
					Moto_Set_GSpeed(motor,(int16_t)speed_m);
					break;
				case key4:
					Input_DoubleValue(&rotation,"Goal Position");
					Input_IntValue(&speed_m,"Goal Speed");
					W_MOTOR_OLD_FUNC(motor, (float)rotation , (int16_t)speed_m , CMD_SET_PSG);
					break;
				case key5:
					Moto_Clear_NPos(motor);
					break;
				case key6:
					W_MOTOR_OLD_FUNC(motor, 0 , 0 , CMD_INIT_CAN);
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/****************************冯哈伯控制盒相关函数******************************/
void Motor_FHB_Init(void)
{
	Com_Printf(5,"CONTMOD\r");
	Com_Printf(5,"SOR0\r");
	//Com_Printf(5,"BAUD115200\r");
	Com_Printf(5,"EN\r");
	Delay_ms(10);
	Com_Printf(5,"v0\r");
}


/*
 * 函数名: Motor_Test_FHB_Uart
 * 描  述: UART控制FHB电机控制盒
 * 输  入: 
 * 输  出: 无
 * 调  用: 外部调用
 */
void Motor_Test_FHB_Uart(ID_NUMDEF motor)
{
	rt_uint32_t key_value;
	double rotation=0;
	int speed_m=0;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("=====FHB Motor%d=====",motor-W_MOTOR1_OLD_ID+1);
		LCD_SetXY(0,1);
		LCD_WriteString("1.STOP ");
		LCD_SetXY(0,2);
		LCD_WriteString("2.Set Position");
		LCD_SetXY(0,3);
		LCD_WriteString("3.Set Speed");
		LCD_SetXY(0,4);
		LCD_WriteString("4.Enable");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Com_Printf(5,"v0\r");
					break;
				case key2:
					Input_DoubleValue(&rotation,"Goal Position");
					Com_Printf(5,"LA%d\r",(int)(rotation*28200));
					Delay_ms(10);
					Com_Printf(5,"M\r");
					break;
				case key3:
					Input_IntValue(&speed_m,"Goal Speed");
					Com_Printf(5,"v%d\r",speed_m);
					break;
				case key4:
					Motor_FHB_Init();
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/****************************测试函数******************************/

/*
 * 函数名: Motor_View
 * 描  述: 电机测试
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Motor_View(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======Motor========");
		LCD_SetXY(0,1);
		LCD_Printf("1.Test motor 1");
		LCD_SetXY(0,2);
		LCD_Printf("2.Test motor 2");	
		LCD_SetXY(0,3);
		LCD_Printf("3.Test motor 3");	
		LCD_SetXY(0,4);
		LCD_Printf("4.Test motor 4");	
		LCD_SetXY(0,5);
		LCD_Printf("5.Test motor 5");
		LCD_SetXY(0,6);
		LCD_Printf("6.Motor Init");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Motor_Test(W_MOTOR1_OLD_ID);
					break;
				case key2:
					Motor_Test(W_MOTOR2_OLD_ID);
					break;
				case key3:
					Motor_Test(W_MOTOR3_OLD_ID);
					break;
				case key4:
					Motor_Test(W_MOTOR4_OLD_ID);
					break;
				case key5:
					
					break;
				case key6:
					Motor_Init();
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
1:
*/

/*
 * 函数名: Motor_Init
 * 描  述: 电机初始化
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Motor_Init(void)
{
	int16_t speed = 1800;
	
	Moto_Set_GSpeed(W_MOTOR1_OLD_ID, -speed);
	Moto_Set_GSpeed(W_MOTOR2_OLD_ID, -speed);
	Moto_Set_GSpeed(W_MOTOR3_OLD_ID, -2*speed);
	
	while(M_IO_READ(W_MOTOR1_OLD_ID,1) || M_IO_READ(W_MOTOR2_OLD_ID,3) || M_IO_READ(W_MOTOR3_OLD_ID,3))
		Delay_ms(5);
	
//	while(M_IO_READ(W_MOTOR1_OLD_ID,1) || M_IO_READ(W_MOTOR2_OLD_ID,3))
//		Delay_ms(5);
	
	Moto_Clear_NPos(W_MOTOR1_OLD_ID);
	Moto_Clear_NPos(W_MOTOR2_OLD_ID);
	Moto_Clear_NPos(W_MOTOR3_OLD_ID);
	
}

void MotorLED_On(void)
{
	Relay_Set(21);
}

void MotorLED_Off(void)
{
	Relay_Clear(21);
}

void MotorLED_Toggle(void)
{
	if(relay_attain(21))
		Relay_Clear(21);
	else
		Relay_Set(21);
}



/**********************************END OF FILE*********************************/
