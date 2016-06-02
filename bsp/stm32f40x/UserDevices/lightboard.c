
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"

/*
 * 函数名: LB_Count
 * 描  述: 数灯的个数，从低位开始数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
u8 LB_Count(u8 port)
{
	u32 data =LB_recv[port-1].v;
	int i;
	
	for(i=0;i<24;i++)
	{
		if(data>>i & 1)
			break;
	}
	return i;
}
/*
 * 函数名: LB_adjust
 * 描  述: 灯板校准
 * 输  入: 场地参数
 * 输  出: 无
 * 调  用: 外部调用
 */
void LB_adjust(u8 data)
{
	rt_uint32_t key_value;
	
	Com_SendByte(5,0xee);
	Com_SendByte(2,0xee);
	
	Delay_ms(5);
	
	Com_SendByte(5,data);
	Com_SendByte(2,data);
	
	while(1)
	{
		if((LB_recv[0].v == 0x55) && (LB_recv[1].v == 0x55))
			break;
		Delay_ms(5);
	}
	
	Com_SendByte(5,0xa4);
	Com_SendByte(2,0xa4);
	
	LCD_Clear();
	LCD_SetXY(7,2);
	LCD_WriteString("Done!");
	LCD_SetXY(0,3);
	LCD_WriteString("Press any key to continue");
	
	if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
}

void LB_adjust_s(u8 id,u8 data)//单个校准
{
	u8 temp[] = {5,2};//0号灯板对应UART5的灯板，也对应LB_recv[0]
	
	Com_SendByte(temp[id],0xee);
	Delay_ms(5);
	Com_SendByte(temp[id],data);
	
	while(1)
	{
		if(LB_recv[id].v == 0x55)
			break;
		Delay_ms(5);
	}
	
	Com_SendByte(temp[id],0xa4);
	
	LCD_Clear();
	LCD_SetXY(7,2);
	LCD_WriteString("Done!");
	LCD_SetXY(0,3);
	LCD_WriteString("Press any key to continue");
}

/*
 * 函数名: 
 * 描  述: 
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void LB_Init(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("==red==     ==blue==");
		LCD_SetXY(0,1);
		LCD_WriteString("1.Red");
		LCD_SetXY(0,2);
		LCD_WriteString("2.Red_Y");
		LCD_SetXY(0,3);
		LCD_WriteString("3.Red_W");
		LCD_SetXY(10,1);
		LCD_WriteString("4.Blue");
		LCD_SetXY(10,2);
		LCD_WriteString("5.Blue_Y");
		LCD_SetXY(10,3);
		LCD_WriteString("6.Blue_W");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					LB_adjust(0xa1);
					break;
				case key2:
					LB_adjust(0xa2);
					break;
				case key3:
					LB_adjust_s(0,0xa3);
					if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
					LCD_Clear();
					LB_adjust_s(1,0xa3);
					break;
				case key4:
					LB_adjust(0xb1);
					break;
				case key5:
					LB_adjust(0xb2);
					break;
				case key6:
					LB_adjust_s(0,0xb3);
					if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK);
					LCD_Clear();
					LB_adjust_s(1,0xb3);
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: LB_State
 * 描  述: 更改灯板的工作状态
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void LB_State(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("1.Red-Yellow");
		LCD_SetXY(0,1);
		LCD_WriteString("2.Red-Yellow-White");
		LCD_SetXY(0,2);
		LCD_WriteString("3.Blue-Yellow");
		LCD_SetXY(0,3);
		LCD_WriteString("4.Blue-Yellow-White");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Com_SendByte(2,0x05);
					Com_SendByte(5,0x05);
					break;
				case key2:
					Com_SendByte(2,0x06);
					Com_SendByte(5,0x06);
					break;
				case key3:
					Com_SendByte(2,0x07);
					Com_SendByte(5,0x07);
					break;
				case key4:
					Com_SendByte(2,0x08);
					Com_SendByte(5,0x08);
					break;
				case keyback:
					return;
			}
			LCD_Clear();
			LCD_SetXY(0,0);
			LCD_WriteString("Done!");
			Delay_ms(1000);
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: 
 * 描  述: 
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void LB_Dis(u32 data)
{

}


/*
 * 函数名: 
 * 描  述: 
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void LB_Test(void)
{
	double error_D;
	double error_A;
	double Vout_A=0;							//PID调整后角度输出值
 	double Vout_D=0;							//PID调整后距离输出值
	
	u8 temp_state;
	u8 acc_flag = 1;
	
	static double V_max=800;
	double acc=300;
	double dec=300;
	
	struct Input_ListValue Values[]=
	{
	 {&Pid_List[8].p,	TYPE_FP64,	"AP"},
	 {&Pid_List[8].i,   TYPE_FP64,	"AI"},
	 {&Pid_List[8].d,	TYPE_FP64,	"AD"},
	 {&Pid_List[9].p,	TYPE_FP64,	"DP"},
	 {&Pid_List[9].i,	TYPE_FP64,	"DI"},
	 {&Pid_List[9].d,	TYPE_FP64,	"DD"},
	 {&V_max,			TYPE_FP64,	"V_max"},
	};
	
	PID_Clear();
	GPS_Clear();
	Input_List(Values,7);
	
	temp_state = Button8_State;
	
	while(1)
	{
		//加速阶段
		if(acc_flag)
			Speed_Y += acc;
		if(Speed_Y>V_max)
			Speed_Y = V_max;
		
		//减速阶段
		if(!acc_flag)
			Speed_Y -= dec/1000;
		if(Speed_Y<0)
			Speed_Y = 0;
		
		if(Button8_State!=temp_state)
			acc_flag = 0;
		
		error_D = LB_Count(1)+LB_Count(2)-2*8;
		error_A = LB_Count(1)-LB_Count(2);
		
		Vout_D = PID(error_D,Pid_List[9]);
		Vout_A = 100*PID(error_A,Pid_List[8]);
		
		if(Vout_A>300)
			Vout_A=300;
		if(Vout_A<-300)
			Vout_A=-300;
		
		//为四轮赋速度
		
		Speed_X = Vout_D;
		Speed_Rotation = Vout_A - Speed_Y*100.0/1200.0;
		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("LB1:%d  LB2:%d",LB_Count(1),LB_Count(2));
		LCD_SetXY(0,1);
		LCD_Printf("LB1+LB2:%d",LB_Count(1)+LB_Count(2));
		LCD_SetXY(0,2);
		LCD_Printf("LB1-LB2:%d",LB_Count(1)-LB_Count(2));
		LCD_SetXY(0,3);
		LCD_Printf("error_D:%g",error_D);
		LCD_SetXY(0,4);
		LCD_Printf("error_A:%g",error_A);		
		LCD_SetXY(0,5);
		LCD_Printf("Vout_D:%g",Vout_D);
		LCD_SetXY(0,5);
		LCD_Printf("Vout_A:%g",Vout_A);
		LCD_SetXY(0,6);
		LCD_Printf("x:%g",Speed_X);
		LCD_SetXY(10,6);
		LCD_Printf("y:%g",Speed_Y);
		LCD_SetXY(0,7);
		LCD_Printf("w:%g",Speed_Rotation);
		
		Delay_ms(5);
	}
}

/**********************************END OF FILE*********************************/
