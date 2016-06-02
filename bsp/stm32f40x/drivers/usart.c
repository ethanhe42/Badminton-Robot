


/** @attention : uart 的分配
  * ---------------------------------------------------------
  * | @uartx |      in              |       out             |
  * ---------------------------------------------------------
  * | @uart1 |                      |     shell & faulhaber | 
  * | @uart2 |                      |                       |
  * | @uart3 | laser rangefinder    |                       |
  * | @uart4 | lightboart           |                       |
  * | @uart6 | handle data          |                       |
  * ---------------------------------------------------------
  */
  
  
//#include "stm32f4xx_uart.h"
#include "stm32f4xx_usart.h"
#include "usart.h"
#include "board.h"
#include "dma.h"
#include <serial.h>



typedef enum
{PA, PB, PC, PD, PE, PF, PG, PH, PI}eGPIO_t;

typedef struct
{
    u8 uartx;
    u8 rx_pin;
    u8 tx_pin;
    eGPIO_t rx_gpiox;
    eGPIO_t tx_gpiox;
    u32 baud;
}uart_t;

uart_t uarts[] = 
{   //uartx rx_pin  tx_pin  rx_gpiox    tx_gpiox    baud
    {1,     7,      6,      PB,         PB,         115200    },  //uart5
    {2,     3,      2,      PA,         PA,         115200  },  //uart1
    {3,     11,     10,     PB,         PB,         115200  },  //uart3
    {4,     1,      0,      PA,         PA,         115200  },  //uart2
    {6,     7,      6,      PC,         PC,         115200   },  //uart4
};

/*  串口时钟配置 */
static void _uart_rcc_init(uart_t *uart)
{
    u32 rcc_apb;
    
    assert_param(uart->uartx>=1 && uart->uartx<=6);
    
    /* enable clock */
    if(uart->uartx==1)
    {
        rcc_apb = ((uint32_t)0x00000010);
        RCC->APB2ENR |= rcc_apb;
    }
    else if(uart->uartx ==6)
    {
        rcc_apb = ((uint32_t)0x00000020);
        RCC->APB2ENR |= rcc_apb;
    }
    else
    {
        rcc_apb = RCC_APB1Periph_USART2 <<(uart->uartx -2);
        RCC->APB1ENR |= rcc_apb;
    }
}

/* 串口 gpio 口配置 */ 
static void _uart_gpio_init(uart_t *uart)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_TypeDef *rx_gpiox = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400*uart->rx_gpiox);
    GPIO_TypeDef *tx_gpiox = (GPIO_TypeDef *)(AHB1PERIPH_BASE + 0x0400*uart->tx_gpiox);
    u16 rx_pin_bit = 1<< uart->rx_pin;
    u16 tx_pin_bit = 1<< uart->tx_pin;
    
    u8 af;
    
    assert_param(uart->uartx>=1 && uart->uartx<=6);
    
    /* enble gpio clock */
    RCC_AHB1PeriphClockCmd((u32)(1<<uart->rx_gpiox), ENABLE);
    RCC_AHB1PeriphClockCmd((u32)(1<<uart->tx_gpiox), ENABLE);
    
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    
    GPIO_InitStructure.GPIO_Pin = rx_pin_bit;
    GPIO_Init(rx_gpiox, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = tx_pin_bit;
    GPIO_Init(tx_gpiox, &GPIO_InitStructure);     

    /* Connect alternate function */
    af = (uart->uartx <=3) ? 0x07 : 0x08;
    GPIO_PinAFConfig(rx_gpiox, (u8)uart->rx_pin, af);
    GPIO_PinAFConfig(tx_gpiox, (u8)uart->tx_pin, af);
}

/* 串口中断向量表配置 */
static void _uart_nvic_init(uart_t *uart)
{
    u8 uart_irqns[] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};
    NVIC_InitTypeDef nvic;

    nvic.NVIC_IRQChannel = uart_irqns[uart->uartx -1];
    nvic.NVIC_IRQChannelPreemptionPriority = 1;    
    nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic);
}

/* 串口自身参数配置 */
static void _uart_conf(uart_t *uart)
{
    USART_InitTypeDef uart_init;
    USART_TypeDef *_uart[] = {USART1, USART2, USART3, UART4, UART5, USART6};
    
    uart_init.USART_BaudRate = uart->baud;
	uart_init.USART_WordLength = USART_WordLength_8b;
	uart_init.USART_StopBits = USART_StopBits_1;
	uart_init.USART_Parity = USART_Parity_No;
	uart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(_uart[uart->uartx -1], &uart_init);
    USART_Cmd(_uart[uart->uartx -1], ENABLE);

    //USART_ITConfig(_uart[uart->uartx-1], USART_IT_RXNE, ENABLE);         
}


#if 1 /* serial device data structure defines */
/* -- uart1 -- */

struct stm32_serial_int_rx  uart1_int_rx;
struct stm32_serial_dma_tx  uart1_dma_tx;
struct stm32_serial_device uart1 =
{
	USART1,
	&uart1_int_rx,
	//&uart1_dma_tx,
    RT_NULL,
};
struct rt_device uart1_device;

/* -- uart2 -- */
struct stm32_serial_int_rx  uart2_int_rx;
struct stm32_serial_dma_tx  uart2_dma_tx;
struct stm32_serial_device uart2 =
{
	USART2,
	&uart2_int_rx,
	&uart2_dma_tx,
    //RT_NULL,
};
struct rt_device uart2_device;

/* -- uart3 -- */
struct stm32_serial_int_rx  uart3_int_rx;
struct stm32_serial_dma_tx  uart3_dma_tx;
struct stm32_serial_device uart3 =
{
	USART3,
	&uart3_int_rx,
	&uart3_dma_tx,
    //RT_NULL,
};
struct rt_device uart3_device;

/* -- uart4 -- */
struct stm32_serial_int_rx  uart4_int_rx;
struct stm32_serial_dma_tx  uart4_dma_tx;
struct stm32_serial_device uart4 =
{
	UART4,
	&uart4_int_rx,
	&uart4_dma_tx,
    //RT_NULL,
};
struct rt_device uart4_device;


/* -- uart6 -- */
struct stm32_serial_int_rx  uart6_int_rx;
struct stm32_serial_dma_tx  uart6_dma_tx;
struct stm32_serial_device uart6 =
{
	USART6,
	&uart6_int_rx,
	&uart6_dma_tx,
    //RT_NULL,
};
struct rt_device uart6_device;
#endif


void Uarts_Init(void)
{
    int i;
    void uart_dma_init(u8 uartx);
    
    int uart_num = sizeof(uarts) /sizeof(uarts[0]);
    for(i=0; i<uart_num; i++)
    {
        _uart_rcc_init(&uarts[i]);
        _uart_gpio_init(&uarts[i]);
        _uart_conf(&uarts[i]);
        _uart_nvic_init(&uarts[i]);
    }
    
    
    /* ---- register uart 2 ---- */
/** @attention: uart2用作键盘输入用，不用dma的输入*/
 
	rt_hw_serial_register(&uart2_device, "uart2",
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |  RT_DEVICE_FLAG_DMA_TX /*RT_DEVICE_FLAG_STREAM |*/,
		&uart2);
    
    uart_dma_init(2);
	  USART_DMACmd(USART2, USART_DMAReq_Rx, DISABLE );
	
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);      
	
    /* ---- register uart 3 ---- */
	rt_hw_serial_register(&uart3_device, "uart3",
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |  RT_DEVICE_FLAG_DMA_TX /*RT_DEVICE_FLAG_STREAM |*/,
		&uart3);
    
    uart_dma_init(3); 

    
    /* ---- register uart 4 ---- */
	rt_hw_serial_register(&uart4_device, "uart4",
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |  RT_DEVICE_FLAG_DMA_TX /*RT_DEVICE_FLAG_STREAM |*/,
		&uart4);
    
    uart_dma_init(4); 
//    USART_DMACmd(UART4, USART_DMAReq_Rx, DISABLE );
//	
//    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);   
    
    /* ---- register uart 6 ---- */
	rt_hw_serial_register(&uart6_device, "uart6",
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX |  RT_DEVICE_FLAG_DMA_TX /*RT_DEVICE_FLAG_STREAM |*/,
		&uart6);
    
    uart_dma_init(6); 
    
    /* ---- register uart 1 ---- */
	rt_hw_serial_register(&uart1_device, "uart1",
		RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM/*| RT_DEVICE_FLAG_DMA_TX*/  ,
		&uart1);
    
    uart_dma_init(1);    
// 	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
}


void Com_SendByte(u8 uartx, u8 byte)
{
    /* 不存在uart5，当参数uartx为5时，实际上是板子上丝印为6的uart输出 */
    USART_TypeDef *_uarts[] = {USART2, UART4, USART3, USART6, USART1};
    USART_TypeDef *uart = _uarts[uartx-1];
    u32 count = 0;
    assert_param(uartx>=1 && uartx<=6);
    USART_SendData(uart, byte);
    
    while (USART_GetFlagStatus(uart, USART_FLAG_TXE)==RESET && count<0xffff)
    {
        count++;
    }
    
    if(!(count<0xfff))
        assert_param(0);
}

void Com_Puts(u8 uartx, char * str)
{
    /* 不存在uart5，当参数uartx为5时，实际上是板子上丝印为6的uart输出 */
    USART_TypeDef *_uarts[] = {USART2, UART4, USART3, USART6, USART1};
    USART_TypeDef *uart = _uarts[uartx-1];
    u32 count = 0;

    assert_param(uartx>=1 && uartx<=6);

    while (*str)
    {
        count = 0;
        USART_SendData(uart, *str++);
        while (USART_GetFlagStatus(uart, USART_FLAG_TXE)==RESET && count<0xffff)
        {
            count++;
        }
        if(!(count<0xfff))
            assert_param(0);
    }
}


extern int vsprintf(char *string, char *format, va_list param);
void Com_Printf(u8 uartx, char *fmt, ...)
{
    va_list ap;
    char string[128];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    va_end(ap);
    Com_Puts(uartx, string);
}



/* 用于显示灯板的数据 */
static char itob_buf[33]={'\0'};
char *itob(u32 val) /* int to bin */
{
    int i=0;
    
    itob_buf[32] = '\0'; /* 最后一位是字符串的结束符 */
    for(i=0; i<24; i++)
    {
        if((val>>i) & 0x00000001)
            itob_buf[23-i] = '1';
        else
            itob_buf[23-i] = '0';
    }
    return &itob_buf[0];
}



// #include "dma.h"
// #include "dma_rx_isr.h"
// /** 
//  * @laser @interface 
//  */
// s32 LaserRead(u8 laserx)
// {
//    static u8 laser_req = 0xaa;
//    static u32 last_recv_cnt = 0;
//    
//    union 
//    {
//         u8 d[4];
//         s32 v;
//    }laser_data;
//    
//    /* ask data */
//    Dma_Out(3, &laser_req, 1);
//    
//    /* take out laser data */
//    laser_data.d[0] = Laser_recv[laserx-1].data[0];
//    laser_data.d[1] = Laser_recv[laserx-1].data[1];
//    laser_data.d[2] = Laser_recv[laserx-1].data[2];
//    laser_data.d[3] = 0;
//    
//    /* check sum */
//    if(Laser_recv[laserx-1].sum != (Laser_recv[laserx-1].data[0] + Laser_recv[laserx-1].data[1] + Laser_recv[laserx-1].data[2] + 0xbb))
//    {
//         /* show transmit err here */
//    }
//    
//    /* is data updated ? */
//    if(last_recv_cnt == *p_laser_count)
//    {
//         /* show data isn't updated here */
//    }
//    last_recv_cnt = *p_laser_count;
//    
//    return laser_data.v;
// }
