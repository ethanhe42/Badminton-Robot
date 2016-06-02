
#include "stm32f4xx.h"
#include "dma.h"
#include "rtthread.h"
#include <serial.h>
#include "dma_rx_isr.h"
#include "rtservice.h"

uint8_t aTxBuffer[] = "aTxBuff";

extern struct rt_device uart1_device;
extern struct rt_device uart2_device;
extern struct rt_device uart3_device;
extern struct rt_device uart4_device;
extern struct rt_device uart6_device;

/* reference manual page 164 */
uart_dma_t uart_dmas[] = 
{ //uartx   rxdmax  rx_streamx  rx_channel  tx_dmax tx_streamx  tx_channel  recv_size   recv_addr   	recv_head   p_rx_cnt		dev				对应的丝印上的UART
//    {1,     2,      5,          4,          2,      7,          4,          6,        &LB_recv[0],    0x55,       0,				&uart1_device},//5 用作灯板1
    {1,     2,      5,          4,          2,      7,          4,          8,          &mt2,           0x55,       0,				&uart1_device},//5 用作灯板1
    {2,     1,      5,          4,          1,      6,          4,          6,          0,   			      0x55,       0,				&uart2_device},//1 用作键盘
    {3,     1,      1,          4,          1,      3,          4,          9,         	&HandleData_Y,	0xc8,       0,				&uart3_device},//3 用作手柄
    {4,     1,      2,          4,          1,      4,          4,          5,          &Laser_recv[1], 0xbb,       0,				&uart4_device},//2 用作激光测距2
    {6,     2,      2,          5,          2,      6,          5,          9,          &HandleData_X,	0xc8,       0,				&uart6_device},//4 用作手柄
};

/**@attention 手柄接收计数值的指针*/
u32 *handle_count_y = &uart_dmas[2].rx_cnt;
u32 *handle_count = &uart_dmas[4].rx_cnt;

/**@attention 激光测距的计数值*/
//u32 *p_laser_count = &uart_dmas[2].rx_cnt;

static   DMA_Stream_TypeDef *streams[][8] =
        {
            {   DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3, DMA1_Stream4, 
                                            DMA1_Stream5, DMA1_Stream6, DMA1_Stream7    },
            {   DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3, DMA2_Stream4, 
                                            DMA2_Stream5, DMA2_Stream6, DMA2_Stream7    },
        };
        
static   uint32_t itflags[8] =
            {   DMA_IT_TCIF0, DMA_IT_TCIF1, DMA_IT_TCIF2, DMA_IT_TCIF3, DMA_IT_TCIF4,
                                            DMA_IT_TCIF5, DMA_IT_TCIF6, DMA_IT_TCIF7    };                                            


uart_dma_t *_get_uart_dma(u8 uartx)
{
    uart_dma_t *p_uart_dma;
    u8 _num = sizeof(uart_dmas)/sizeof(uart_dmas[0]);
    
    int i=0;
    for(i=0; i<_num; i++)
        if(uart_dmas[i].uartx == uartx)
        {
            p_uart_dma = &uart_dmas[i];
            return p_uart_dma;
        }
	return 0;
}

/* dma 接收的 buff ，在中断中将数据从buff中取出来 */
static u8 uart_dma_buf[7][20]={0};


DMA_Stream_TypeDef *_get_stream(u8 dmax, u8 streamx)
{
    return streams[dmax -1][streamx];
}


void uart_dma_init(u8 uartx)
{
    
    USART_TypeDef *_uarts[] = 
        { USART1, USART2, USART3, UART4, UART5, USART6 };
    DMA_InitTypeDef  dma_init;
    uart_dma_t *p_uart_dma;
    struct stm32_serial_device *_uart;  /* serial device 的 user_data 域 */

    DMA_Stream_TypeDef *rx_streams, *tx_streams;

    p_uart_dma =  _get_uart_dma(uartx);

    rx_streams = streams[p_uart_dma->rx_dmax -1][p_uart_dma->rx_streamx];
    tx_streams = streams[p_uart_dma->tx_dmax -1][p_uart_dma->tx_streamx];  

    /* ---- Enable the DMA clock ---- */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    /* ---- Configure DMA Initialization Structure ---- */
    dma_init.DMA_BufferSize           = 6; /* 0x01 ~ 0x10000 16k */
    dma_init.DMA_FIFOMode             = DMA_FIFOMode_Disable ;
    dma_init.DMA_FIFOThreshold        = DMA_FIFOThreshold_1QuarterFull ;
    dma_init.DMA_MemoryBurst          = DMA_MemoryBurst_Single ;
    dma_init.DMA_MemoryDataSize       = DMA_MemoryDataSize_Byte;
    dma_init.DMA_MemoryInc            = DMA_MemoryInc_Enable;
    dma_init.DMA_Mode                 = DMA_Mode_Normal;
    dma_init.DMA_PeripheralBaseAddr   =(uint32_t)(&(_uarts[uartx-1]->DR)) ;
    dma_init.DMA_PeripheralBurst      = DMA_PeripheralBurst_Single;
    dma_init.DMA_PeripheralDataSize   = DMA_PeripheralDataSize_Byte;
    dma_init.DMA_PeripheralInc        = DMA_PeripheralInc_Disable;
    dma_init.DMA_Priority             = DMA_Priority_VeryHigh;
    
    /* ---- Configure TX DMA ---- */
    dma_init.DMA_Channel              = ((u32)0x02000000) * (u32)p_uart_dma->tx_channel ;
    dma_init.DMA_DIR                  = DMA_DIR_MemoryToPeripheral ;
    dma_init.DMA_Memory0BaseAddr      =(uint32_t)aTxBuffer ;

    DMA_Init(tx_streams, &dma_init);
    
    DMA_Cmd(tx_streams, DISABLE);/* disable */
    DMA_ITConfig(tx_streams, DMA_IT_TC, ENABLE);
    _uart = (struct stm32_serial_device *)p_uart_dma->dev->user_data;
    _uart->dma_tx->dma_channel = tx_streams;
    USART_DMACmd(_uarts[uartx-1], USART_DMAReq_Tx, ENABLE);
    
    /* ---- Configure RX DMA ---- */
    dma_init.DMA_BufferSize           = p_uart_dma->recv_size; 
    dma_init.DMA_Mode                 = DMA_Mode_Circular; 
    dma_init.DMA_Channel              = ((u32)0x02000000) * (u32)p_uart_dma->rx_channel ;
    dma_init.DMA_DIR                  = DMA_DIR_PeripheralToMemory ;
    dma_init.DMA_Memory0BaseAddr      =(uint32_t)&uart_dma_buf[p_uart_dma->uartx][0]; 
    DMA_Init(rx_streams, &dma_init);
    DMA_ITConfig(rx_streams, DMA_IT_TC, ENABLE);   
    USART_DMACmd(_uarts[uartx-1], USART_DMAReq_Rx, ENABLE);
    
    /* Enable DMA */
    DMA_Cmd(rx_streams, ENABLE);
    
    
    /* ---- nvic ---- */
    {
        u8 irqn[][8] = {   {11, 12, 13, 14, 15, 16, 17, 47}, 
                            {56, 57, 58, 59, 60, 68, 69, 70}    };
        NVIC_InitTypeDef nvic;

        nvic.NVIC_IRQChannel = irqn[p_uart_dma->tx_dmax -1][p_uart_dma->tx_streamx];
        nvic.NVIC_IRQChannelPreemptionPriority = 1;
        nvic.NVIC_IRQChannelSubPriority = 1;
        nvic.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic);
                            
        nvic.NVIC_IRQChannel = irqn[p_uart_dma->rx_dmax -1][p_uart_dma->rx_streamx];
        nvic.NVIC_IRQChannelPreemptionPriority = 1;
        nvic.NVIC_IRQChannelSubPriority = 1;
        nvic.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic);                    
    }
}


#include "rtthread.h"
#include "serial.h"

void uart_dma_rx_isr(u8 uartx);
void uart_dma_tx_isr(u8 uartx);
extern void rt_hw_serial_dma_tx_isr(struct rt_device *device);

/* ---- uart 1 tx irq_handler ---- */
void DMA2_Stream7_IRQHandler(void)
{
    uart_dma_tx_isr(1);
}


/* ---- uart 1 rx irq_handler ---- */
void DMA2_Stream5_IRQHandler(void)
{
    uart_dma_rx_isr(1);
}

/* ---- uart 2 tx irq_handler ---- */
void DMA1_Stream6_IRQHandler(void)
{
    uart_dma_tx_isr(2);
}

/* ---- uart 2 rx irq_handler ---- */
void DMA1_Stream5_IRQHandler(void)
{
    uart_dma_rx_isr(2);
}

/* ---- uart 3 tx irq_handler ---- */
void DMA1_Stream3_IRQHandler(void)
{

    uart_dma_tx_isr(3);
}



/* ---- uart 3 rx irq_handler ---- */
void DMA1_Stream1_IRQHandler(void)
{
    uart_dma_rx_isr(3);
}


/* ---- uart 4 tx irq_handler ---- */
void DMA1_Stream4_IRQHandler(void)
{
    uart_dma_tx_isr(4);
}



/* ---- uart 4 rx irq_handler ---- */
void DMA1_Stream2_IRQHandler(void)
{
    uart_dma_rx_isr(4);
}

/* ---- uart 6 tx irq_handler ---- */
void DMA2_Stream6_IRQHandler(void)
{
    uart_dma_tx_isr(6);
}



/* ---- uart 6 rx irq_handler ---- */
void DMA2_Stream2_IRQHandler(void)
{
    uart_dma_rx_isr(6);
}
  


void rt_hw_serial_dma_rx_isr(u8 uartx);
void uart_dma_rx_isr(u8 uartx)
{
    DMA_Stream_TypeDef *stream;
    uint32_t itflag;
    uart_dma_t *p_uart_dma;
    
    p_uart_dma =  _get_uart_dma(uartx);
    stream = streams[p_uart_dma->rx_dmax -1][p_uart_dma->rx_streamx];
    itflag = itflags[p_uart_dma->rx_streamx];
    
    /* enter interrupt */
    rt_interrupt_enter();
    
    if(DMA_GetITStatus(stream, itflag))
    {
        /* transmission complete, invoke serial dma tx isr */
        rt_hw_serial_dma_rx_isr(uartx);
    }
    
    DMA_ClearITPendingBit(stream, itflag);
    DMA_Cmd(stream, ENABLE);
    
    rt_interrupt_leave();
}


                                           
void uart_dma_tx_isr(u8 uartx)
{
    DMA_Stream_TypeDef *stream;
    uint32_t itflag;
    uart_dma_t *p_uart_dma;
    
    p_uart_dma =  _get_uart_dma(uartx);
    stream = streams[p_uart_dma->tx_dmax -1][p_uart_dma->tx_streamx];
    itflag = itflags[p_uart_dma->tx_streamx];
    
    /* enter interrupt */
    rt_interrupt_enter();
    
    if(DMA_GetITStatus(stream, itflag))
    {
        /* transmission complete, invoke serial dma tx isr */
        rt_hw_serial_dma_tx_isr(p_uart_dma->dev);
    }
    
    DMA_ClearITPendingBit(stream, itflag);
    
    rt_interrupt_leave();
}

/* dma 输出标准字符串 */
/** @attention  1：这个函数不能在中断函数中调用
                2：在线程中不能连续写两句，若需要写两句，在两句之间加入 delay
  * 
  */
void Dma_Printf(u8 uartx, char *fmt, ...)
{
    uart_dma_t *p_uart_dma;
	
	u8 uart_map[] = {2, 4, 3, 6, 1};
    va_list args;
    rt_size_t length;
    static char rt_log_buf[RT_CONSOLEBUF_SIZE];

	uartx = uart_map[uartx-1];
    p_uart_dma =  _get_uart_dma(uartx);
    
    va_start(args, fmt);
    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
    if (length > RT_CONSOLEBUF_SIZE - 1)
        length = RT_CONSOLEBUF_SIZE - 1;
    
    p_uart_dma->dev->write(p_uart_dma->dev, 0, rt_log_buf, length);

    va_end(args);
}

/**  输出一串 u8 类型的数
     与Dma_Printf函数的区别是，Dma_Printf函数只能输出字符串
  */
void Dma_Out(u8 uartx, u8 *u8ptr, u8 len)
{
    uart_dma_t *p_uart_dma;
	u8 uart_map[] = {2, 4, 3, 6, 1};
	
	uartx = uart_map[uartx-1];
    p_uart_dma =  _get_uart_dma(uartx);
    p_uart_dma->dev->write(p_uart_dma->dev, 0, u8ptr, len);
}

