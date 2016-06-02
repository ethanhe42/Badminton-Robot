
#ifndef __DMA_h_
#define __DMA_h_
#include "stm32f4xx.h"
#include "rtthread.h"

typedef struct
{
    u8 uartx;
    u8 rx_dmax;
    u8 rx_streamx;
    u8 rx_channel;
    u8 tx_dmax;
    u8 tx_streamx;
    u8 tx_channel;
    u8 recv_size;
    void *recv_addr;
    u8 recv_head;
	u32 rx_cnt;
    struct rt_device *dev;
}uart_dma_t;

extern u32 *handle_count_y;
extern u32 *handle_count;
//extern u32 *p_laser_count;

void Dma_Printf(u8 uartx, char *fmt, ...);
void Dma_Out(u8 uartx, u8 *cptr, u8 len);
DMA_Stream_TypeDef *get_rStream(rt_device_t device);
DMA_Stream_TypeDef *_get_stream( u8 dmax, u8 streamx);
uart_dma_t *_get_uart_dma(u8 uartx);

#endif

