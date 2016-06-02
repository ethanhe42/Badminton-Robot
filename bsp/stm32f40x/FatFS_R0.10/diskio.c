/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "sdcard.h"		/* Example: MMC/SDC contorl */
#include "stm324xg_eval_sdio_sd.h"
#include "rtthread.h"

/* Definitions of physical drive number for each media */
//#define ATA		0
//#define MMC		1
//#define USB		2
#define SD_CARD 0

extern SD_CardInfo SDCardInfo;
SD_Error status;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	switch (pdrv) {
	case SD_CARD:
		status = SD_Init();
		if(status != SD_OK)
			return STA_NOINIT;
		else
			return RES_OK;
//	case ATA :
//		result = ATA_disk_initialize();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case MMC :
//		result = MMC_disk_initialize();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case USB :
//		result = USB_disk_initialize();
//
//		// translate the reslut code here
//
//		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	switch (pdrv) {
	case SD_CARD:
		status = SD_GetCardInfo(&SDCardInfo);
		if(status != SD_OK)
			return STA_NOINIT;
		else
			return RES_OK;
//	case ATA :
//		result = ATA_disk_status();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case MMC :
//		result = MMC_disk_status();
//
//		// translate the reslut code here
//
//		return stat;
//
//	case USB :
//		result = USB_disk_status();
//
//		// translate the reslut code here
//
//		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	SD_Error Status;
	
	if( !count )
	{    
		return RES_PARERR; 
	}
	switch (pdrv) {	
	case SD_CARD:
    if(count==1)              
    {       
		Status =  SD_ReadBlock( buff ,sector<< 9 , 512 );
    }                                                
    else                    
    {   
		Status = SD_ReadMultiBlocks( buff ,sector<< 9 ,512,count);
    }                                                
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    if(Status == SD_OK)
    {
      return RES_OK;
    }
    else
    {
      return RES_ERROR;
    }   
//	case ATA :
//		// translate the arguments here
//
//		result = ATA_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case MMC :
//		// translate the arguments here
//
//		result = MMC_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case USB :
//		// translate the arguments here
//
//		result = USB_disk_read(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	SD_Error Status;
	
	if( !count )
	{    
		return RES_PARERR; 
	}
	switch (pdrv)
	{
	case SD_CARD:
				if(count==1)              
				{   
					rt_enter_critical();
					Status = SD_WriteBlock( (uint8_t *)(&buff[0]) ,sector << 9 , 512); 
					rt_exit_critical();
				}                                                
				else                  
				{    
					rt_enter_critical();
					Status = SD_WriteMultiBlocks( (uint8_t *)(&buff[0]) ,sector << 9 ,512,count);
					rt_exit_critical();
				} 
				/* Check if the Transfer is finished */
				Status = SD_WaitWriteOperation();
				rt_enter_critical();
				while(SD_GetStatus() != SD_TRANSFER_OK);	
				rt_exit_critical();
				if(Status == SD_OK)
				{
				   return RES_OK;
				}
				else
				{
				   return RES_ERROR;
				}
//	case ATA :
//		// translate the arguments here
//
//		result = ATA_disk_write(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case MMC :
//		// translate the arguments here
//
//		result = MMC_disk_write(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
//
//	case USB :
//		// translate the arguments here
//
//		result = USB_disk_write(buff, sector, count);
//
//		// translate the reslut code here
//
//		return res;
	}
	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) {
	case SD_CARD:
		switch (cmd)
		{
// 			case CTRL_SYNC:
// 				// no synchronization to do since not buffering in this module
// 				return RES_OK;
// 			case GET_SECTOR_SIZE:
// 				*(WORD*)buff = 512;
// 				return RES_OK;
// 			case GET_SECTOR_COUNT:
// 				*(DWORD*)buff = SDCardInfo.CardCapacity/512;
// 				return RES_OK;
// 			case GET_BLOCK_SIZE:
// 				*(DWORD*)buff = 512;
// 				return RES_OK;
			  case CTRL_SYNC :	      
				return RES_OK;
			  case GET_SECTOR_COUNT : 
				*(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
				return RES_OK;
			  case GET_BLOCK_SIZE :
				*(WORD*)buff = SDCardInfo.CardBlockSize;
				return RES_OK;	
			  case CTRL_POWER :
				break;
			  case CTRL_LOCK :
				break;
			  case CTRL_EJECT :
				break;
			  /* MMC/SDC command */
			  case MMC_GET_TYPE :
				break;
			  case MMC_GET_CSD :
				break;
			  case MMC_GET_CID :
				break;
			  case MMC_GET_OCR :
				break;
			  case MMC_GET_SDSTAT :
				break;	
      }
		return RES_PARERR;
//	case ATA :
//		// pre-process here
//
//		result = ATA_disk_ioctl(cmd, buff);
//
//		// post-process here
//
//		return res;
//
//	case MMC :
//		// pre-process here
//
//		result = MMC_disk_ioctl(cmd, buff);
//
//		// post-process here
//
//		return res;
//
//	case USB :
//		// pre-process here
//
//		result = USB_disk_ioctl(cmd, buff);
//
//		// post-process here
//
//		return res;
	}
	return RES_PARERR;
}
#endif

DWORD get_fattime (void)
{
	return 0;
}







