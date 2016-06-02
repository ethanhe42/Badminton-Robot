#ifndef _SONIC_H
#define _SONIC_H
	#include "stm32f4xx.h"
	
	#define	SONIC1	0x01
	#define	SONIC2	0x02
	#define	SONIC3	0x04
	#define	SONIC4	0x08
	#define	SONIC5	0x10
	#define	SONIC6	0x20
	
	
	typedef struct
	{
		uint8_t  state;
		uint8_t  count;
		uint16_t data;
	}sonic_t;
	
	extern sonic_t sonic_data[6];
	
	void SONICx_ENABLE(uint8_t SONICx);
	void SONICx_DISABLE(uint8_t SONICx);
	

#endif
