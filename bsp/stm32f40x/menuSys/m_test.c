/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/


/*
 * 函数名: DataBack
 * 描  述: 数据回传函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void DataBackTest(void)
{
//	rt_uint32_t key_value;
	
	while(1)
	{
		
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======DataBack======");
		LCD_SetXY(0,1);
		
		#if DataBackEnable
		LCD_WriteString("DataNumber:");
		LCD_WriteInt(DataBack_GetCount());
		LCD_SetXY(0,2);
		LCD_WriteString("Press OK to send");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case keyOK:
					LCD_Clear();
					LCD_SetXY(5,3);
					LCD_WriteString("Waiting...");
				
					DataBack_Write(DataBack_GetCount());
					DataBack_Send();
					Com_SendByte(2,'$');
				
					LCD_Clear();
					LCD_SetXY(7,3);
					LCD_WriteString("Done~!");
					Delay_ms(500);
					break;
				case keyback:
					return;
			}
		}
		#else 
			LCD_WriteString("Please Enable the   DataBack Function");
		#endif
		Delay_ms(10);
	}
}




/*
 * 函数名: CameraTest
 * 描  述: 摄像头测试函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */

void CameraTest(void)
{
	rt_uint32_t key_value;	
	 
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_Printf("1.ShowInfo");
		LCD_SetXY(0,1);
		LCD_Printf("2.Handover");
		LCD_SetXY(0,2);
		LCD_Printf("3.Pole");
		LCD_SetXY(0,3);
		LCD_Printf("4.Circle_sta");
		LCD_SetXY(0,4);
		LCD_Printf("5.WhiteTape");
		LCD_SetXY(0,5);
		LCD_Printf("6.Recode");
		LCD_SetXY(0,6);
		LCD_Printf("7.Circle_dyn");
		LCD_SetXY(0,7);
		LCD_Printf("8.TestData");
	 
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
          ShowInfo();				
				  break;
				
				case key2:
        	Handover(TEST,NONE);
				  break;
				
				case key3:
		      while(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) != RT_EOK)
					{
						LCD_Clear();
						LCD_SetXY(0,0);
						LCD_Printf("1.NEW");
						LCD_SetXY(0,1);
						LCD_Printf("2.OLD");
						Delay_ms(20);
					}
					switch(key_value)
					{
						case key1:
							PolePutTest();
						  break;
						case key2:
							PolePut(TEST);
						  break;
            default:
							break;
					}
				  break;
					
				case key4:
					CamCircle_Static(TEST,NONE);
				  break;
				
				case key5:
					CamWhiteTape(TEST);
				  break;
				
				case key6:
					CamRecode(NONE,NONE,TEST);
				
				case key7:
          CamCircle_Dynamic(TEST);				
				  break;
				
				case key8:
					ShowData();
				  break;
				
				case keyback:
					return;
			}
		}
		Delay_ms(20);
  }
}

/**@addToFile : "m_sys.c"
  *---------------------------------------*
  | @attention : menu func                |
  *---------------------------------------*/

void sys_ImgSave(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Img Save======");
		LCD_SetXY(0,1);
		LCD_WriteString("1.b_seesaw");
		LCD_SetXY(0,2);
		LCD_WriteString("2.b_polewalk");
    LCD_SetXY(0,3);
		LCD_WriteString("3.b_whitetape");
		LCD_SetXY(0,4);
		LCD_Printf("4.Handover");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
        u8 res;
                
				case key1:
                    /* save image from camera1 ,file name is "b_seesaw.txt" */
                    res = Save_Img("b_seesaw", 1);
                    _show_res(res);
					break;
				
				case key2:
					res = Save_Img("b_polewalk", 4);
                    _show_res(res);
					break;
				
        case key3:
					res = Save_Img("b_whitetape", 2);
                    _show_res(res);
				  res = Save_Img("b_whitetape", 1);
                    _show_res(res);
					break;		
								
				case key4:
					res = Save_Img("Handover",5);
				            _show_res(res);
				  break;
				
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}



/*
 * 函数名: LaserTest
 * 描  述: 激光测距测试函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void LaserTest(void)
{
	while(1)
	{
		LCD_Clear();
		//LCD_SetXY(0,0); LCD_Printf("Laser_Dist:%d",LaserRead(1));
     //   LCD_SetXY(0,1); LCD_Printf("Laser_Dist:%d",LaserRead(2));
		Delay_ms(10);
	}
}

/*
 * 函数名: HallTest
 * 描  述: 
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void HallTest(void)
{
	rt_uint32_t key_value;
	extern u8 Hall_Count;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("========Hall========");
		LCD_SetXY(0,1);
		LCD_WriteString("1.SS_init");
		LCD_SetXY(0,2);
		LCD_WriteString("2.open hand_SS");
		LCD_SetXY(0,3);
		LCD_WriteString("3.leg_on");
		LCD_SetXY(0,4);
		LCD_WriteString("4.leg_off");
		LCD_SetXY(0,5);
		LCD_WriteString("5.open hand_SW");
		LCD_SetXY(0,6);
		LCD_WriteString("6.PW_init");
		LCD_SetXY(0,7);
		LCD_WriteInt(Hall_Count-1);

		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Hall_Send(1);
					break;
				case key2:
					Hall_Send(2);	
					break;
				case key3:
					Hall_Send(3);
					break;
				case key4:
					Hall_Send(4);
					break;
				case key5:
					Hall_Send(5);
					break;
				case key6:
					Hall_Send(6);
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

#include "LightBoard.h"
/*
 * 函数名: LBTest
 * 描  述: 灯板测试函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void LBTest(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=========LB=========");
		LCD_SetXY(0,1);
		LCD_WriteString("LB1:");
		LCD_WriteInt(LB_Count(1));
		LCD_SetXY(10,1);
		LCD_WriteString("LB2:");
		LCD_WriteInt(LB_Count(2));
		LCD_SetXY(0,2);
		LCD_WriteString("LB1:");
		LCD_Printf("%s",itob(LB_recv[0].v));
		LCD_SetXY(0,4);
		LCD_WriteString("LB2:");
		LCD_Printf("%s",itob(LB_recv[1].v));
		LCD_SetXY(0,6);
		LCD_WriteString("1.LB_Init");
		LCD_SetXY(10,6);
		LCD_WriteString("2.LB_State");
		LCD_SetXY(0,7);
		LCD_WriteString("3.LB_Test");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					LB_Init();
					break;
				case key2:
					LB_State();
					break;
				case key3:
					LB_Test();
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
}

/*
 * 函数名: sys_Sonic
 * 描  述: 超声波数据显示
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void SonicTest(void)
{
	uint8_t i;
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======Sonic========");
		for(i=1;i<7;i++)
		{
			LCD_SetXY(0,i);
			if(sonic_data[i-1].state == 1)
			{
				LCD_Printf("%d.set",i);
			}
			else
			{
				LCD_Printf("%d.clear",i);
			}
			LCD_SetXY(8,i);
			LCD_Printf("%d",sonic_data[i-1].count);
			LCD_SetXY(12,i);
			LCD_Printf("%d",sonic_data[i-1].data);
		}
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					
				case key2:
					
				case key3:
                    
				case key4:
					if(sonic_data[key_value-1].state == 1)
					{
						SONICx_DISABLE((uint8_t)((0x01)<<(key_value-1)));
						sonic_data[key_value-1].state = 0;
					}
					else
					{
						SONICx_ENABLE((uint8_t)((0x01)<<(key_value-1)));
						sonic_data[key_value-1].state = 1;
					}
					break;
				case key5:
					
				case key6:
					if(sonic_data[key_value-3].state == 1)
					{
						SONICx_DISABLE((uint8_t)((0x01)<<(key_value-3)));
						sonic_data[key_value-3].state = 0;
					}
					else
					{
						SONICx_ENABLE((uint8_t)((0x01)<<(key_value-3)));
						sonic_data[key_value-3].state = 1;
					}
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
/*
 * 函数名: RadarTest
 * 描  述: 激光雷达数据显示
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void RadarTest(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("=======Radar========");
		LCD_SetXY(0,1);
		LCD_Printf("%d",sonic_data[0].data);
		LCD_SetXY(10, 1);
		LCD_Printf("%d",sonic_data[0].count);
		LCD_SetXY(0,2);
		LCD_Printf("%d",sonic_data[3].data);
		LCD_SetXY(10,2);
		LCD_Printf("%d",sonic_data[3].count);
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
                    
					break;
				case key2:
					
					break;
				case key3:
                    
					break;
				case key4:
					
					break;
				case key5:
					
					break;
				case key6:
					
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


/*
 * 函数名: owenTest
 * 描  述: 测试函数
 * 输  入: 无
 * 输  出: 无
 * 调  用: 外部调用
 */
void owenTest(void)
{
	rt_uint32_t key_value;
	static FIL fil_owen;
	extern int Handle_Off_Count;
		extern int Handle_Off_Count2;
	double speed;
	
/*->*/owen_test:
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("1.Mecanum");
		LCD_SetXY(0,1);
		LCD_WriteString("2.close f_pid");
		LCD_SetXY(0,2);
		LCD_WriteString("3.SD_Record");
		LCD_SetXY(0,3);
		LCD_Printf("4.Handle off: %d",Handle_Off_Count);
		
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					goto owen_mecanum;
				case key2:
					SD_Printf_MQ(&fil_pid,"THE END");
					break;
				case key3:
					goto owen_SD_Record;
				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
	
/*->*/owen_mecanum:
	
	Input_DoubleValue(&speed,"Speed_Y");
	
	Wait_Button8_Change();
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("go ahead");
		
		Speed_Y = speed;
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case keyback:
					Speed_Y = 0;
					goto owen_test;
			}
		}
		Delay_ms(10);
	}
	
/*->*/owen_SD_Record:
	
	f_open (&fil_owen, "owen.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("Recording...");
		
		SD_Printf_MQ(&fil_owen,"%f,%f,%f,%f,%f;\r\n",GPS.position.x,GPS.position.y,GPS.radian,Encoder[0].dis,Encoder[1].dis);
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			SD_Printf_MQ(&fil_owen,"THE END");
			
			LCD_Clear();
			LCD_SetXY(0,0);
			LCD_WriteString("file have closeed");
			
			if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_FOREVER) == RT_EOK)
					goto owen_test;
		}
		Delay_ms(10);
	}
}

/********************************    END OF FILE    ***************************/

