#include "t_timer.h"
#include "includes.h"
#include "rtthread.h"
#include "tim.h"
#include "stm32f4xx.h"
#include "usart.h"
//倍率
#define COUNT_MS 1000

ALIGN(RT_ALIGN_SIZE)
static char thread_tim_stack[1024];
struct rt_thread thread_tim;
//struct TIME_State TIMER;

static struct timer
{
	double TIME1;	
	double TIME2;
}M_Struct={0,0};

double M_TIME=0;//主控时间轴变量
double S_TIME=0;//协助控时间轴变量


void TIM3_Syn_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;    
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
  TIM_TimeBaseStructure.TIM_Period = 42-1;       
  TIM_TimeBaseStructure.TIM_Prescaler = 0;    
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM3, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//打印在显示器上
void sys_Timer(void)
{
	rt_uint32_t key_value;
	
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Timer=======");
		LCD_SetXY(0,1);
		LCD_WriteString("M_TIME:");
		LCD_WriteDouble(M_TIME);
		LCD_SetXY(0,2);
		LCD_WriteString("S_TIME:");
		LCD_WriteDouble(S_TIME);
		LCD_SetXY(0,6);
		LCD_WriteString("1.Init_Timer");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					Init_Timer();
					break;
				case keyback:
					return;
			}
		}
		Delay_ms(1);
	}
}


void gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    GPIO_SetBits(GPIOA, GPIO_Pin_2);
}
void text_time(void)
{
	double TEXT_TIME=1000000;
	double TEXT_TIME2=2000000;
		while(1)
	{
		
		if(M_TIME==TEXT_TIME)
		{
			LED_Off(3);
			GPIO_ResetBits(GPIOA, GPIO_Pin_2);
			TEXT_TIME=TEXT_TIME+2000000;
		}
		if(M_TIME==TEXT_TIME2)
		{
			LED_On (3);
			GPIO_SetBits(GPIOA, GPIO_Pin_2);
			TEXT_TIME2=TEXT_TIME2+2000000;
		}
	}
}

void Init_Timer(void)
{
	M_TIME = 0;
	S_TIME = 0;
}
	
static void rt_thread_entry_tim(void* parameter)
{	

	RCC_ClocksTypeDef RCC_Clockstructure;
//	TIM3_Syn_Init();
	RCC_GetClocksFreq(&RCC_Clockstructure);

//	gpio_init();
//	text_time();
}


void TIM3_IRQHandler(void)
{

	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		M_TIME++;
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

//要发送本地时间数据函数，调用
/*
void TIME_SEND(struct timer M_Struct)
{
	MOSI_TIME1_T1.f64_data = M_Struct.TIME1;
	Write_Database(W_TIME1_T1_ID);

	MOSI_TIME1_T1.f64_data = M_Struct.TIME2;
	Write_Database(W_TIME1_T2_ID);
}
void TIME_SYN()
{
	double detla=0;
	detla = ((TIMER.TIME1-M_Struct.TIME1)-(TIMER.TIME2-M_Struct.TIME2))/2;
}
*/

int rt_thread_tim_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_tim, 
                            "thread_tim",
                            rt_thread_entry_tim,
                            RT_NULL,
                            &thread_tim_stack[0], sizeof(thread_tim_stack),
							7, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_tim);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************END OF FILE************************/
