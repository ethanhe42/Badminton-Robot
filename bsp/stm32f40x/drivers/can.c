/**
  ******************************************************************************
  * @file    CAN/CAN_Networking/can.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-January-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "can.h"

#define CAN1_BAUDRATE 1000
#define CAN2_BAUDRATE 1000

//是否使能相应can总线，及其FIFO
#define USE_CAN1_R0	1
#define USE_CAN1_R1	1
#define USE_CAN2_R0	1
#define USE_CAN2_R1	1

void Can_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
#if  USE_CAN1_R0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if  USE_CAN1_R1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if  USE_CAN2_R0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x2;
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_Init(&NVIC_InitStructure);
#endif
#if  USE_CAN2_R1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

void Can_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Configure CAN1 and CAN2 IOs **********************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
#if USE_CAN1_R0 || USE_CAN1_R1
	/* Connect CAN pins to AF9 */
	GPIO_PinAFConfig(GPIOB , GPIO_PinSource8, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOB , GPIO_PinSource9, GPIO_AF_CAN1); 
	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
	
#if USE_CAN2_R0 || USE_CAN2_R1
	/* Connect CAN pins to AF9 */
	GPIO_PinAFConfig(GPIOB , GPIO_PinSource12, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB , GPIO_PinSource13, GPIO_AF_CAN2); 
	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
	
}

void CAN_CAN_Config(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	
	/* Enable CAN clock */
#if USE_CAN1_R0 || USE_CAN1_R1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);
	CAN_DeInit(CAN1);
#endif
#if USE_CAN2_R0 || USE_CAN2_R1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2 , ENABLE);
	CAN_DeInit(CAN2);
#endif
	


	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;//失能时间触发通讯模式
	CAN_InitStructure.CAN_ABOM = DISABLE;//失能自动离线管理
	CAN_InitStructure.CAN_AWUM = DISABLE;//失能自动唤醒模式
	
	CAN_InitStructure.CAN_NART = DISABLE;//失能非自动重传输模式,即自动重传
	
	
	CAN_InitStructure.CAN_RFLM = DISABLE;//失能接收FIFO锁定模式,即FIFO溢出时新数据覆盖
	CAN_InitStructure.CAN_TXFP = ENABLE; //使能发送FIFO优先级,报文顺序由发送顺序决定 DISABLE:ID决定
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;		   //BTR-SJW 重新同步跳跃宽度 1个时间单元

	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;		   //BTR-TS1 时间段1 占用了2个时间单元
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;		   //BTR-TS1 时间段2 占用了3个时间单元
	
	//总线波特率,波特率计算请参见 F4手册“24.7.7 Bit timing”
#if CAN1_BAUDRATE == 1000 /* 1MBps */
  CAN_InitStructure.CAN_Prescaler =6;			   //BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+3+3)/6=1Mbps
#elif CAN1_BAUDRATE == 500 /* 500KBps */
  CAN_InitStructure.CAN_Prescaler =12;
#elif CAN1_BAUDRATE == 250 /* 250KBps */
  CAN_InitStructure.CAN_Prescaler =24;
#elif CAN1_BAUDRATE == 125 /* 125KBps */
  CAN_InitStructure.CAN_Prescaler =48;
#endif

#if USE_CAN1_R0 || USE_CAN1_R1
	CAN_Init(CAN1, &CAN_InitStructure);
#endif
#if USE_CAN2_R0 || USE_CAN2_R1
	CAN_Init(CAN2, &CAN_InitStructure);
#endif
}

void Can_FILTER_Config(void)
{
	int8_t i=0;
	int8_t can1_filter_bank = 0,can2_filter_bank = 15;
	int8_t can1_fifo0_num = 0,can1_fifo1_num = 0;
	int8_t can2_fifo0_num = 0,can2_fifo1_num = 0;
	uint16_t temp1[14*4]={0};
	uint16_t temp2[14*4]={0};
	uint16_t temp3[14*4]={0};
	uint16_t temp4[14*4]={0};
	
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
#if USE_CAN2_R0 || USE_CAN2_R1
	CAN_SlaveStartBank(15);
#endif
	
	for(i=0;i<Can_Data_Num;i++)
	{
		if(Can_Database[i].Data_type==READ_ONLY)
		{
			if(Can_Database[i].Channel==1)
			{
				if(Can_Database[i].Fifo_num==CAN_Filter_FIFO0)
				{
					temp1[can1_fifo0_num] = (uint16_t)(Can_Database[i].Data_ID)<<5;
					can1_fifo0_num++;
				}else
				{
					temp2[can1_fifo1_num] = (uint16_t)(Can_Database[i].Data_ID)<<5;
					can1_fifo1_num++;
				}
				
			}
			else if(Can_Database[i].Channel==2)
			{
				if(Can_Database[i].Fifo_num==CAN_Filter_FIFO0)
				{
					temp3[can2_fifo0_num] = (uint16_t)(Can_Database[i].Data_ID)<<5;
					can2_fifo0_num++;
				}else
				{
					temp4[can2_fifo1_num] = (uint16_t)(Can_Database[i].Data_ID)<<5;
					can2_fifo1_num++;
				}
			}
		}
	}
	
	for(i=0;i<(3 - (can1_fifo0_num-1)%4);i++)
	{
		temp1[can1_fifo0_num+i] = temp1[can1_fifo0_num-1];
	}
	for(i=0;i<(3 - (can1_fifo1_num-1)%4);i++)
	{
		temp2[can1_fifo1_num+i] = temp2[can1_fifo1_num-1];
	}
	for(i=0;i<(3 - (can2_fifo0_num-1)%4);i++)
	{
		temp3[can2_fifo0_num+i] = temp3[can2_fifo0_num-1];
	}
	for(i=0;i<(3 - (can2_fifo1_num-1)%4);i++)
	{
		temp4[can2_fifo1_num+i] = temp4[can2_fifo1_num-1];
	}
	
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	
    if(can1_fifo0_num>0)
	{
		for(i=0;i<=((can1_fifo0_num-1) / 4);i++)
		{
			CAN_FilterInitStructure.CAN_FilterNumber = can1_filter_bank;
			can1_filter_bank++;
			CAN_FilterInitStructure.CAN_FilterIdHigh	=	temp1[4*i+0];
			CAN_FilterInitStructure.CAN_FilterIdLow		=	temp1[4*i+1];
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=	temp1[4*i+2];
			CAN_FilterInitStructure.CAN_FilterMaskIdLow	=	temp1[4*i+3];
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
			CAN_FilterInit(&CAN_FilterInitStructure);
		}
	}
	
	if(can1_fifo1_num>0)
	{
		for(i=0;i<=((can1_fifo1_num-1) / 4);i++)
		{
			CAN_FilterInitStructure.CAN_FilterNumber = can1_filter_bank;
			can1_filter_bank++;
			CAN_FilterInitStructure.CAN_FilterIdHigh	=	temp2[4*i+0];
			CAN_FilterInitStructure.CAN_FilterIdLow		=	temp2[4*i+1];
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=	temp2[4*i+2];
			CAN_FilterInitStructure.CAN_FilterMaskIdLow	=	temp2[4*i+3];
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;
			CAN_FilterInit(&CAN_FilterInitStructure);
		}
	}
	
	if(can2_fifo0_num>0)
	{
		for(i=0;i<=((can2_fifo0_num-1) / 4);i++)
		{
			CAN_FilterInitStructure.CAN_FilterNumber = can2_filter_bank;
			can2_filter_bank++;
			CAN_FilterInitStructure.CAN_FilterIdHigh	=	temp3[4*i+0];
			CAN_FilterInitStructure.CAN_FilterIdLow		=	temp3[4*i+1];
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=	temp3[4*i+2];
			CAN_FilterInitStructure.CAN_FilterMaskIdLow	=	temp3[4*i+3];
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
			CAN_FilterInit(&CAN_FilterInitStructure);
		}
	}
	
	if(can2_fifo1_num>0)
	{
		for(i=0;i<=((can2_fifo1_num-1) / 4);i++)
		{
			CAN_FilterInitStructure.CAN_FilterNumber = can2_filter_bank;
			can2_filter_bank++;
			CAN_FilterInitStructure.CAN_FilterIdHigh	=	temp4[4*i+0];
			CAN_FilterInitStructure.CAN_FilterIdLow		=	temp4[4*i+1];
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh=	temp4[4*i+2];
			CAN_FilterInitStructure.CAN_FilterMaskIdLow	=	temp4[4*i+3];
			CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO1;
			CAN_FilterInit(&CAN_FilterInitStructure);
		}
	}
}

void Can_IT_Config(void)
{
#if  USE_CAN1_R0
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
#endif
#if  USE_CAN1_R1
	CAN_ITConfig(CAN1, CAN_IT_FMP1, ENABLE);
#endif
#if  USE_CAN2_R0
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
#endif
#if  USE_CAN2_R1
	CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);
#endif
}

void CANx_Init(void)
{
	Hash_Table_init();
	Can_NVIC_Config();
	Can_GPIO_Config();
	CAN_CAN_Config();
	Can_FILTER_Config();
	Can_IT_Config();
}



/* 存储的信息
 1：日期
 2：时间
 3：当前线程
 4：GPS
 5：ID号，data，length */
static FIL can_send_file;
static void _can_senddata(CAN_TypeDef *canx, CanTxMsg *tx)
{
    u8 wait_time = 0;
    
    while(CAN_Transmit(canx, tx)==CAN_NO_MB && wait_time<6)
    {
        wait_time ++;
        Delay_ms(5);
    }
    /* delay 30ms */
    if(wait_time>=6)
    {
        extern struct rt_thread *rt_current_thread;
        int i;
        
        LCD_Clear();
        LCD_SetXY(2, 3);
        LCD_Printf("can send timeout!");
        
        f_open (&can_send_file, "can_errInfo.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
        f_lseek(&can_send_file, can_send_file.fsize);
        SD_Printf2(&can_send_file, "");
        SD_Printf2(&can_send_file, "-----------------------------------------------");
        SD_Printf2(&can_send_file, "-->date:%s", __DATE__);
        SD_Printf2(&can_send_file, "-->time:%s", __TIME__);
        SD_Printf2(&can_send_file, "-->tick:%d", rt_tick_get());
        SD_Printf2(&can_send_file, "-->thread:%s", rt_current_thread->name);
        SD_Printf2(&can_send_file, "-->GPS:");
        SD_Printf2(&can_send_file, "   >x:%f", GPS.position.x);
        SD_Printf2(&can_send_file, "   >y:%f", GPS.position.y);
        SD_Printf2(&can_send_file, "   >a:%f", GPS.radian);
        SD_Printf2(&can_send_file, "-->txMsg:");
        SD_Printf2(&can_send_file, "   >ID  :%d", tx->StdId);
        SD_Printf2(&can_send_file, "   >len :%d", tx->DLC);
        SD_Printf2(&can_send_file, "   >data", tx->DLC);
      for(i=0; i<tx->DLC; i++)
        SD_Printf(&can_send_file, "     %x", tx->Data[i]);
        SD_Printf2(&can_send_file, "");
        SD_Printf2(&can_send_file, "-----------------------------------------------");
        SD_Printf2(&can_send_file, "");
        
        f_close(&can_send_file);
        Delay_ms(500);
    }
}
void Can_SendData(CanTxMsg* TxM , uint8_t Can_Channel)
{
	if(Can_Channel==1)
        _can_senddata(CAN1, TxM); 

	if(Can_Channel==2)
		_can_senddata(CAN2, TxM); 
}
