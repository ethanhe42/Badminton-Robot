
#include "io.h"
#include "dma.h"
#include "usart.h"
#include "dma_rx_isr.h"

/* light board */
LB_t LB_recv[2];

/* manual */
Handle_Data HandleData_X,HandleData_Y;

/* laser rangefinder */
laser_t Laser_recv[2];

/* motor */
motor_t mt2;

void rt_hw_serial_dma_rx_isr(u8 uartx)
{
	//rt_uint32_t level;
    uart_dma_t *p_uart_dma = _get_uart_dma(uartx);
    u8 f_size = p_uart_dma->recv_size;  //frame size
    
    /* 保存的数据的地址，由dma通道数据结构获得 */
    DMA_Stream_TypeDef *rx = _get_stream(p_uart_dma->rx_dmax, p_uart_dma->rx_streamx);
    
    char *addr = (char *)rx->M0AR;
    DMA_Cmd(rx, DISABLE);
    
    /* 判断数据有没有对齐 ,对齐且长度为帧长度时写入缓冲区，否则下次刷掉 */
    if(*addr==p_uart_dma->recv_head && rx->NDTR==f_size)   
    {   
        int i=0;
        char *rbuf = (char *)p_uart_dma->recv_addr;
        
        assert_param(rbuf != RT_NULL);
        
				/** @receive @correctly */
				LED_Toggle(2);
        
				for(i=0; i<p_uart_dma->recv_size; i++)
            *(rbuf+i) = *((char *)(rx->M0AR) + i);
		
		/* 接收数据计数值累加 */
		p_uart_dma->rx_cnt ++;
    }
    else if(rx->NDTR == (uint32_t)f_size)
    {/* 长度为帧长，说明是第一次出现数据传输错误 */  
        
        /* 重新设置dma接收的长度，达到对齐的目的 */
        /* 找帧头 */
        int i=0;
        //int fh_pos=0; /* frame head position */
        for(i=0; i<f_size; i++)
        {
            if(*(addr+i)==p_uart_dma->recv_head)
                break;
        }
        
        if(i==f_size)
        {/* 该帧中无帧头，不做改动继续接受下一帧*/
        }
        else
        {/* 帧头在该帧的中间*/
            rx->NDTR += i;
        }
        
    }
    else
    {   /* 长度不为帧长，应该是传输错误的第二次 */
        /* 找帧头 */
        int i=0;
        //int fh_pos=0; /* frame head position */
        for(i=0; i<f_size; i++)
        {
            if(*(addr+i)==p_uart_dma->recv_head)
                break;
        }
        if(i==f_size)
        {/* 该帧中无帧头, 表示出错*/
        }
        else
        {/* 帧头在该帧的中间 */
            if((rx->NDTR -i)==f_size)
            {/* 确认是出错后的第二次中断 */
                rx->NDTR = f_size;
            }
        }
    }
    DMA_Cmd(rx, ENABLE);
}



void laser_dma_rx_isr(u8 uartx)
{
	//rt_uint32_t level;
    uart_dma_t *p_uart_dma = _get_uart_dma(uartx);
    u8 f_size = p_uart_dma->recv_size;  //frame size
    
    /* 保存的数据的地址，由dma通道数据结构获得 */
    DMA_Stream_TypeDef *rx = _get_stream(p_uart_dma->rx_dmax, p_uart_dma->rx_streamx);
    
    char *addr = (char *)rx->M0AR;
    DMA_Cmd(rx, DISABLE);
    
    /* 判断数据有没有对齐 ,对齐且长度为帧长度时写入缓冲区，否则下次刷掉 */
    if(*addr==p_uart_dma->recv_head && rx->NDTR==f_size)   
    {   
        int i=0;
        char *rbuf = (char *)p_uart_dma->recv_addr;
        
        assert_param(rbuf != RT_NULL);
        
				/** @receive @correctly */
				LED_Toggle(2);
        
				for(i=0; i<p_uart_dma->recv_size; i++)
            *(rbuf+i) = *((char *)(rx->M0AR) + i);
		
		/* 接收数据计数值累加 */
		p_uart_dma->rx_cnt ++;
    }
    else if(rx->NDTR == (uint32_t)f_size)
    {/* 长度为帧长，说明是第一次出现数据传输错误 */  
        
        /* 重新设置dma接收的长度，达到对齐的目的 */
        /* 找帧头 */
        int i=0;
        //int fh_pos=0; /* frame head position */
        for(i=0; i<f_size; i++)
        {
            if(*(addr+i)==p_uart_dma->recv_head)
                break;
        }
        
        if(i==f_size)
        {/* 该帧中无帧头，不做改动继续接受下一帧*/
        }
        else
        {/* 帧头在该帧的中间*/
            rx->NDTR += i;
        }
        
    }
    else
    {   /* 长度不为帧长，应该是传输错误的第二次 */
        /* 找帧头 */
        int i=0;
        //int fh_pos=0; /* frame head position */
        for(i=0; i<f_size; i++)
        {
            if(*(addr+i)==p_uart_dma->recv_head)
                break;
        }
        if(i==f_size)
        {/* 该帧中无帧头, 表示出错*/
        }
        else
        {/* 帧头在该帧的中间 */
            if((rx->NDTR -i)==f_size)
            {/* 确认是出错后的第二次中断 */
                rx->NDTR = f_size;
            }
        }
    }
    DMA_Cmd(rx, ENABLE);
}
