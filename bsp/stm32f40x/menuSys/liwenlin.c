#include "liwenlin.h"
#include "includes.h"
#include "t_lcd_refresh.h"


int K=1;
extern	int16_t Speed_M ;
int M1_speed=6000,



        M2_speed=150;
float M1_pos1=5,//M1 起始位置
      M1_pos2=8,//M1 终点位置
		  M2_pos1=12,//M2 初始角度
		  M2_pos2=-1.653,//M2 转过角度
			pos=0,
			adjust=0.4;  //修正值
			

int delay=40;//M1 M2连续动作的延时

void Up_motor(void)
{
		rt_uint32_t key_value;
	
    static u8 flag=0;
	int i=0;


	  
	while(1)
	{
		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Up motor======");
		LCD_SetXY(0,1);
		LCD_WriteString("1.prepare");
		LCD_SetXY(0,2);
		LCD_WriteString("2.hit");
		LCD_SetXY(0,3);
		LCD_WriteString("3.set");
		LCD_SetXY(0,4);
		LCD_WriteString("4.test M2:");		
		LCD_SetXY(0,5);
		LCD_WriteString("5.hit2");	
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case 1:
	      	prepare();
				break;
				case 2:
          hit1();
				break;
				case 3:
					set();
					break;
				case 4:
          test();
					break;
				case key5:
					hit2();
					break;
				case keyback:
					           return;
				default:
						break;
				}
		}

		Delay_ms(10);
	}
	
		
}
void Cylinder(void)
{
	rt_uint32_t key_value;
	static u8 flag[3]={1,1,1};
	while(1)
	{
		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======cylinder======");
		LCD_SetXY(0,1);
		LCD_WriteString("1.gun");	
		LCD_SetXY(0,2);
		LCD_WriteString("2.fire");
		LCD_SetXY(0,3);
		LCD_WriteString("3.hand");		
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case 1:
					if(flag[0])
					{         flag[0]=0;
									    GunOn();
					}
					else
					{
						flag[0]=1;
						GunOff();	
					}
					break;
				case 2:
					if(flag[1])
					{
						flag[1]=0;
						FireOn();
					}
					else
					{
						flag[1]=1;
						FireOff();
					}
					break;
				case 3:
					if(flag[2])
					{
						flag[2]=0;
						HandOpen();
					}
					else
					{
						flag[2]=1;
						HandClose();
					}
					break;
				case keyback:
					return;
			}
		}

		Delay_ms(10);
	}
	
	
}
void Sa_St(void)
{
	
	rt_uint32_t key_value;
	uint8_t st_num=8;
	uint8_t sa_num=5;
	int i,j;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======SA-ST========");
		
		for(i=1;i<=st_num;i++)
		{
			LCD_SetXY(((i-1)%3)*7,(i-1)/3+1);
			LCD_Printf("ST%d:%d",i,ST_READ(i) );
		}
		
		j=(i-1)/3+2;
		
		for(i=1;i<=sa_num;i++)
		{
			LCD_SetXY(((i-1)%3)*7,j+(i-1)/3+1);
			LCD_Printf("SA%d:%d",i,SA_READ(i) );
		}
		
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
void stepper(void)
{rt_uint32_t key_value;
	static u8 dir=0;
	int step=0;
	while(1)
	{
		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======stepper======");
		LCD_SetXY(0,1);
		LCD_WriteString("1.direction");	
		LCD_SetXY(0,2);
		LCD_WriteString("2.set free");
		LCD_SetXY(0,3);
		LCD_WriteString("3.step on");		
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					    dir=~dir;
				      W_STEPPER_FUNC(dir,0,0);
				break;
				case key2:
				      W_STEPPER_FUNC(dir,1,0);
				break;
				case key3:
					Input_IntValue(&step,"step");
				  W_STEPPER_FUNC(dir,0,(u32)step);
				break;
				case keyback:
					return;
			}
		}
	}
				
	
	
	
}
void ST(void);
void key(void)
{
	rt_uint32_t key_value;
	uint8_t st_num=8;
	uint8_t sa_num=5;

	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======key========");
		LCD_SetXY(0,1);
		LCD_Printf("key value:");
		if(Button1_Up)
			LCD_Printf("key value:%d",1);
		if(Button2_Up)
			LCD_Printf("key value:%d",2);
		if(Button3_Up)
			LCD_Printf("key value:%d",3);
		if(Button4_Up)
			LCD_Printf("key value:%d",4);
		if(Button5_Up)
			LCD_Printf("key value:%d",5);
		if(Button6_Up)
			LCD_Printf("key value:%d",6);
		if(Button7_On)
			LCD_Printf("key value:%d",7);
		if(Button8_On)
			LCD_Printf("key value:%d",8);
		if(Button9_On)
			LCD_Printf("key value:%d",9);
		if(Button10_On)
			LCD_Printf("key value:%d",10);
		
		
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

void set(void)
{
	rt_uint32_t key_value;
		while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======key========");
		LCD_SetXY(0,1);
		LCD_Printf("1.set M1_pos1   2.set M1_pos2");
		LCD_SetXY(0,2);
		LCD_Printf("3.set M2_pos_int   4.set M2_rotation");
		LCD_SetXY(0,3);
		LCD_Printf("5.set M1_speed  6..set M2_speed");	
		LCD_SetXY(0,4);
		LCD_Printf("7.set delay   8.set adjust");		
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Input_FloatValue(&M1_pos1,"M1_pos1");
					break;
				case key2:
					Input_FloatValue(&M1_pos2,"M1_pos2");
					break;
				case key3:
					Input_FloatValue(&M2_pos1,"M2_pos1");
					break;
				case key4:
					Input_FloatValue(&M2_pos2,"M2_pos2");
					break;
				case key5:
					Input_IntValue(&M1_speed,"M1 speed");
					break;
				case key6:
					Input_IntValue(&M2_speed,"M2 speed");
					break;
				case key7:
					Input_IntValue(&delay,"delay");
					break;
				case key8:
					Input_FloatValue(&adjust,"adjust");
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
	
}

extern u8 view_flag2;

void prepare(void)
{
	MISO_SWITCH[0]=0x00;
	W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, (float)M1_pos1 , (int16_t)700 , CMD_SET_PSG);
	Moto_Set_GSpeed(W_MOTOR1_OLD_ID,(int16_t)500);
	
	while(!MISO_SWITCH[0])
	{			
	}	
	Delay_ms(5);	
//	while(!view_flag2)
//		Delay_ms(10);
//	view_flag2=0;
	Moto_Stop(W_MOTOR1_OLD_ID);
	MISO_SWITCH[0]=0x00;
	Delay_ms(10);
	M1_pos2=Motor_Pos[0]+3;
}



extern u8 view_flag;

void hit1(void)
{
	W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, (float)pos+M2_pos2, (int16_t) M2_speed , CMD_SET_PSG);
	Delay_ms(delay);
	//W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, (float)M1_pos2, (int16_t) M1_speed , CMD_SET_PSG);
	pos+=M2_pos2;
}
void test(void)
{
	W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, (float)M2_pos1, (int16_t)(500) , CMD_SET_PSG);
	pos=M2_pos1;
}

void hit2(void)
{
	W_FLAG_FUNC();
	W_MOTOR_OLD_FUNC(W_MOTOR2_OLD_ID, (float)pos+M2_pos2, (int16_t)M2_speed , CMD_SET_PSG);
	//while(!MISO_SWITCH[1])
//		Delay_ms(1);
//	while(!view_flag)
//		Delay_ms(10);
//	view_flag=0
	
	Delay_ms(delay);
	//W_MOTOR_OLD_FUNC(W_MOTOR1_OLD_ID, (float)M1_pos2, (int16_t) M1_speed , CMD_SET_PSG);
	MISO_SWITCH[0]=0x00;
  pos+=M2_pos2;
	
}

