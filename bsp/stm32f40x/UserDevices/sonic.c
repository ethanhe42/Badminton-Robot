#include "includes.h"

sonic_t sonic_data[6] = 
{
	{0,0,0},
	{0,0,0},
	{0,0,0},
	{0,0,0},
	{0,0,0},
	{0,0,0},
};			//在接收数据的S_SONIC_FUNC中累加计数和赋新值

void SONICx_ENABLE(uint8_t SONICx)
{
    if((SONICx&0x0f) != 0)
    {
        MOSI_SONIC1_CMD=(uint8_t)((MOSI_SONIC1_CMD) | (SONICx));
        Write_Database(W_SONIC1_ID);
    }else
    {
        MOSI_SONIC2_CMD=(uint8_t)((MOSI_SONIC2_CMD) | (SONICx>>4));
        Write_Database(W_SONIC2_ID);
    }
	
}

void SONICx_DISABLE(uint8_t SONICx)
{
    if((SONICx&0x0f) != 0)
    {
        MOSI_SONIC1_CMD=(uint8_t)((MOSI_SONIC1_CMD) & (~SONICx));
        Write_Database(W_SONIC1_ID);
    }else
    {
        MOSI_SONIC2_CMD=(uint8_t)((MOSI_SONIC2_CMD) & (~(SONICx>>4)));
        Write_Database(W_SONIC2_ID);
    }
}



