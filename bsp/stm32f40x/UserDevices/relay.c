/* Includes ------------------------------------------------------------------*/
#include "includes.h"

uint32_t RELAY_VALUE=0;


//=================================================================
/************************************************************************************************
** 函数名称	:	Relay_Multi
** 功能描述	:   set  或 clear 某个继电器端口，可以同时操作几个口
** 输　入 	:	1.set_num  ：要set的端口
**              2.clear_num：要clear的端口
**          	使用示例：想set 12  13 口和clear 15 和44，62口，可以这样用:
**          	Relay(RELAY_NUM(3)|RELAY_NUM(7),RELAY_NUM(15)|RELAY_NUM(44)|RELAY_NUM(62))
** 输　出   :	
** 注意事项 :选择端口最好用这个宏定义： RELAY_LOW(port) 
**************************************************************************************************/
// void Relay_Multi(uint64_t set_num,uint64_t clear_num)
// {
// 	MISO_RELAY64_DATA.u64_data |=  set_num;

// 	MISO_RELAY64_DATA.u64_data &=  ~clear_num;
// 	
// 	RELAY_VALUE = MISO_RELAY64_DATA.u64_data;
// 	
// 	Write_Database(W_RELAY64_ID);
// }

/*单独置位某个端口*/
void Relay_Set(uint8_t port)
{
	if((port>0)&&(port<17))
	{
		MOSI_RELAY1_DATA = (uint8_t)((port-1) | 0x80);
	
		Write_Database(W_RELAY1_ID);
	}else if((port>16)&&(port<33))
	{
		MOSI_RELAY2_DATA = (uint8_t)((port-17) | 0x80);
	
		Write_Database(W_RELAY2_ID);
	}
	RELAY_VALUE |= RELAY_NUM(port);
}

/*单独复位某个端口*/
void Relay_Clear(uint8_t port)
{
	static uint8_t i;
	if((port>0)&&(port<17))
	{
		MOSI_RELAY1_DATA = (port-1);
		i &= 0x12;
		i++;
		
		Write_Database(W_RELAY1_ID);
	}else if((port>16)&&(port<33))
	{
		MOSI_RELAY2_DATA = (port-17);
		
		Write_Database(W_RELAY2_ID);
	}
	RELAY_VALUE &= ~RELAY_NUM(port);
}


/*
 * 函数名: Realy_View
 * 描  述: 查看继电器信息
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void Relay_View(void)
{
	rt_uint32_t key_value;
	int i,j;
	int page=0;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======Relay========");
		
		for(i=0;i<2;i++)
			for(j=0;j<4;j++)
			{
				LCD_SetXY(i*10,j+1);
				LCD_Printf("%d:",8*page+i*4+j+1);
				if(RELAY_VALUE>>(8*page+i*4+j) & 1)
					LCD_WriteString("Set");
				else
					LCD_WriteString("Clear");
			}
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					if(RELAY_VALUE>>(8*page) & 1)
						Relay_Clear(8*page+1);
					else
						Relay_Set(8*page+1);
					break;
				case key2:
					if(RELAY_VALUE>>(8*page+1) & 1)
						Relay_Clear(8*page+2);
					else
						Relay_Set(8*page+2);
					break;
				case key3:
					if(RELAY_VALUE>>(8*page+2) & 1)
						Relay_Clear(8*page+3);
					else
						Relay_Set(8*page+3);
					break;
				case key4:
					if(RELAY_VALUE>>(8*page+3) & 1)
						Relay_Clear(8*page+4);
					else
						Relay_Set(8*page+4);
					break;
				case key5:
					if(RELAY_VALUE>>(8*page+4) & 1)
						Relay_Clear(8*page+5);
					else
						Relay_Set(8*page+5);
					break;
				case key6:
					if(RELAY_VALUE>>(8*page+5) & 1)
						Relay_Clear(8*page+6);
					else
						Relay_Set(8*page+6);
					break;
				case key7:
					if(RELAY_VALUE>>(8*page+6) & 1)
						Relay_Clear(8*page+7);
					else
						Relay_Set(8*page+7);
					break;
				case key8:
					if(RELAY_VALUE>>(8*page+7) & 1)
						Relay_Clear(8*page+8);
					else
						Relay_Set(8*page+8);
					break;
				case pageup:
					if(page>0) page--;
					break;
				case pagedown:
					if(page<3) page++;
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

u8 relay_attain(u8 port)
{
	return (RELAY_VALUE>>(port-1))&1;
}

/*----------------------------------------------------------------------------*
 *  
 *  下面的电磁阀口每次新车出来都要更新，特别注意:霍尔(HALL)端口
 *  
 *----------------------------------------------------------------------------*/


void HandOpen(void)//交接手  更改于5.4
{
	Relay_Set(20);
}
void HandClose(void)
{
	Relay_Clear(20);
}
void HandTurnRight(void)//旋转调姿  更改于5.4
{
// 	Relay_Clear(19);
	Relay_Set(23);
// 	Delay_ms(500);
// 	Relay_Set(20);
}
void HandTurnLeft(void)
{
// 	Relay_Clear(20);
	Relay_Clear(23);
// 	Delay_ms(500);
// 	Relay_Set(19);
}
void HandPush(void)   //  更改于5.4
{
	//Relay_Set(24);
}
void HandPushOff(void)
{
	//Relay_Clear(24);
}
void HandRaise(void)//俯仰调姿  更改于5.4
{
	Relay_Set(19);
}
void HandFall(void)
{
	Relay_Clear(19);
}
void BufferOn(void)//打开缓冲装置，气缸不压紧  更改于5.4
{
    Relay_Clear(1);
}
void BufferOff(void)
{
	Relay_Set(1);
}
void GunOn(void)//放置大炮  更改于5.4
{
	HandClose();
	Relay_Set(2);
}
void GunOff(void)//收起大炮
{
	HandClose();
	Relay_Clear(2);
}
void FireOn(void)//开火
{
	Relay_Clear(3);
	Relay_Set(4);
	Relay_Clear(5);
	Relay_Set(6);
}
void FireOff(void)
{
	Relay_Clear(4);
	Relay_Set(3);
	Relay_Clear(6);
	Relay_Set(5);
}
void FireClear(void)//缓冲阻挡状态
{
	Relay_Clear(3);
	Relay_Clear(4);
	Relay_Clear(5);
	Relay_Clear(6);
}

/*----------------------------------------------------------------------------*
 *  霍尔传感器通信：先拉高，作为起始位，然后接收4个信号位
 *  编码如下:
 *  0001: 跷跷板初始化（SS）
 *	0010：松板子 & 梅花桩初始化（PW）
 *  0011：夹第一个盘子
 *  0100：松最后一个盘子 & 秋千初始化(SW) & 夹秋千
 *  0101：松开秋千
 *  0110：爬楼梯初始化（JG） & 抓楼梯

 *  0111：
 *  1000：
 *  1001：

 *  1010：
 *	1011：
 *	1011：
 *----------------------------------------------------------------------------*/

/*
 * 函数名: Hall_Send
 * 描  述: 
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
 u8 Hall_Count = 0;
 
void Hall_Send( u8 data)
{
	u8 count ;
	u8 port = 18;
	u8 delay = 20;

	Relay_Set(port);
	Delay_ms( delay );

	for( count = 4 ; count > 0 ; count -- )
	{
		if( data & (1<<(count-1)) )
			Relay_Set(port);
		else
			Relay_Clear(port);
		
		Delay_ms( delay );
	}
	Relay_Clear(port);
	
	Hall_Count = data+1;
}



/**********************************END OF FILE*********************************/
