
/**--------------------------------------------------------+
  | @attention:定时器3和9是底盘电机用的定时器，不可用  |
  +--------------------------------------------------------*/

#include "tim.h"
#include "stm32f4xx.h"

/**--------------------------------------------+
  | 定时器初始化，让定时器每 （1）us 递增 |
  +--------------------------------------------*/
typedef struct 
{
	TIM_TypeDef*    name;
	uint8_t         bus;
	uint32_t        clock;
	enum IRQn       irqn;
	__IO uint32_t   timDelay;
	
}timer;	


timer Timer[]=
{
	{0,0,0,0,0},
	{0,0,0,0,0},
	//{TIM1,2,RCC_APB2Periph_TIM1,TIM1_UP_TIM10_IRQn,0},
	{TIM2,1,RCC_APB1Periph_TIM2,TIM2_IRQn,0},
	{0,0,0,0,0},
	{TIM4,1,RCC_APB1Periph_TIM4,TIM4_IRQn,0},
	{TIM5,1,RCC_APB1Periph_TIM5,TIM5_IRQn,0},
	{TIM6,1,RCC_APB1Periph_TIM6,TIM6_DAC_IRQn,0},
	{TIM7,1,RCC_APB1Periph_TIM7,TIM7_IRQn,0},
	{TIM8,2,RCC_APB2Periph_TIM8,TIM8_UP_TIM13_IRQn,0},
	{0,0,0,0,0}
};	


	
const static u8 tims[] = {1,2,4,5,6,7,8};
const static u8 ntim = sizeof(tims)/sizeof(tims[0]); /* number of timers */



static void _tim_init(u8 timx)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  if(timx==3 || timx==9 || timx==0 || timx==1)
	{
			/* err */
			return;
	}
	
	if(Timer[timx].bus==1)
	{
	  RCC_APB1PeriphClockCmd(Timer[timx].clock, ENABLE);
	  TIM_TimeBaseStructure.TIM_Period = 42-1;//1us
	}
	else
		RCC_APB2PeriphClockCmd(Timer[timx].clock, ENABLE);
	  TIM_TimeBaseStructure.TIM_Period = 84-1;//1us
	
//	TIM_TimeBaseStructure.TIM_Period = 84-1;//1us
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(Timer[timx].name, &TIM_TimeBaseStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = Timer[timx].irqn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_ARRPreloadConfig(Timer[timx].name, ENABLE);
//  TIM_ITConfig(Timer[timx].name, TIM_IT_Update, ENABLE);
  TIM_Cmd(Timer[timx].name, ENABLE);
}



void tim_init(void)
{
    u8 i=0;
    for(i=0; i<ntim; i++)
        _tim_init(tims[i]);
}

/**---------------------------------------+
  |                                       |
  +---------------------------------------*/
void reset_tim(u8 timx)
{
    if(timx==3 || timx==9)
    {
        /* err */
        return;
    }
    
    Timer[timx].name->CNT = 0;
}

/**---------------------------------------+
  | ????????? ??:1/10us     |
  +---------------------------------------*/
u32 get_tim(u8 timx)
{
    if(timx==3 || timx==9)
    {
        /* err */
        return 0;
    }
		
	return Timer[timx].name->CNT;
}

/**---------------------------------------+
  | delay_us()                            |
  +---------------------------------------*/
/* us */
void tim_delay(u8 timx, u32 us)
{
    if(timx==3 || timx==9)
    {
        /* err */
        return;
    }
		TIM_ITConfig(Timer[timx].name, TIM_IT_Update, ENABLE);
		
		Timer[timx].timDelay = us;
	  while(Timer[timx].timDelay != 0x00);
		
		TIM_ITConfig(Timer[timx].name, TIM_IT_Update, DISABLE);
}




void timDelay_Decrement(uint32_t timx)
{
  if (Timer[timx].timDelay != 0)
  { 
    Timer[timx].timDelay--;
  }
}



void TIM1_UP_TIM10_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	  timDelay_Decrement(1);
	}
}
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	  timDelay_Decrement(2);
	}
}
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	  timDelay_Decrement(4);
	}
}
void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	  timDelay_Decrement(5);
	}
}
void TIM6_DAC_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	  timDelay_Decrement(6);
	}
}
void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	  timDelay_Decrement(7);
	}
}
void TIM8_UP_TIM13_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
	  timDelay_Decrement(8);
	}
}
