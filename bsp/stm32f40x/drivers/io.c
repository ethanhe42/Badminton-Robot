/**
  ******************************************************************************
  * @author  fxh_bool
  * @date    2014-2-9

  ******************************************************************************
  * @attention
  ******************************************************************************
  */

#define __IO_C
/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/* Private typedef ----------------------GPIOA-------------------------------------*/
typedef enum
{PA, PB, PC, PD, PE, PF, PG, PH, PI}eGPIO_t;

typedef struct
{
    u8  GPIOx;
    u8  pinx;
}IO_t;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static IO_t leds[] = 
{
    {PI, 6},
    {PI, 7},
    {PE, 2},
    {PE, 4},
};
static IO_t buzs[] = 
{
    {PC, 15},
};

static const int led_num = sizeof(leds) /sizeof(leds[0]);
static const int buz_num = sizeof(buzs) /sizeof(buzs[0]);

/* Private function prototypes -----------------------------------------------*/
void IO_Init(void);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  IO_Init
  */
static void IO_GPIO_Init(IO_t *io)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_TypeDef *gpiox = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400*io->GPIOx);
    u16 pin_bit = 1<<io->pinx;
    
    RCC_AHB1PeriphClockCmd((u32)(1<<io->GPIOx), ENABLE);
    GPIO_InitStructure.GPIO_Pin = pin_bit;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(gpiox, &GPIO_InitStructure); 
    
    GPIO_ResetBits(gpiox, pin_bit);
}


void IO_Init(void)
{
    int i;
    for(i=0; i<led_num; i++)
    {
        IO_GPIO_Init(&leds[i]);
    }
    for(i=0; i<buz_num; i++)
    {
        IO_GPIO_Init(&buzs[i]);
    }
}

static void IO_On(IO_t *io)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400*io->GPIOx);
    u16 pin_bit = 1<< io->pinx;
    
    GPIO_SetBits(GPIOx, pin_bit);
}

static void IO_Off(IO_t *io)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400*io->GPIOx);
    u16 pin_bit = 1<< io->pinx;
    
    GPIO_ResetBits(GPIOx, pin_bit);
}

static void IO_Toggle(IO_t *io)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400*io->GPIOx);
    u16 pin_bit = 1<< io->pinx;
    
    if((GPIOx->ODR & pin_bit) == 0)
        GPIO_SetBits(GPIOx, pin_bit);
    else
        GPIO_ResetBits(GPIOx, pin_bit);   
}

static void IO_Twinkle(IO_t *io)
{
	int i=1000;
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400*io->GPIOx);
    u16 pin_bit = 1<< io->pinx;
    
	GPIO_SetBits(GPIOx, pin_bit);
	while(i--);
	GPIO_ResetBits(GPIOx, pin_bit);   
}

/*-------------------------  LED  --------------------------------------------*/
/* ledx ´Ó 1 ¼ÆÆð */
void LED_On(u8 ledx)
{
    IO_Off(&leds[ledx-1]);
}

void LED_Off(u8 ledx)
{
    IO_On(&leds[ledx-1]);
}

void LED_Toggle(u8 ledx)
{
    IO_Toggle(&leds[ledx-1]);
}

void LED_Twinkle(u8 ledx)
{
    IO_Twinkle(&leds[ledx-1]);
}
/*-------------------------  buzzer  -----------------------------------------*/
void Buz_On(void)
{
    IO_On(&buzs[0]);
}

void Buz_Off(void)
{
    IO_Off(&buzs[0]);
}

void Buz_Toggle(void)
{
    IO_Toggle(&buzs[0]);
}

/********************************    END OF FILE    ***************************/

