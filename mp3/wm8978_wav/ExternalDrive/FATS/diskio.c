/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "sdio_sd.h"
#include "string.h"
/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
extern SD_CardInfo SDCardInfo;
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive [该函数是需要改写的]                                                   */
/* 注意：从函数的代码上来看，所谓的drv这个参数，是用一个数字来代表文件硬件的类型，从其switch语句看
   drv = ATA = 0：表示是IDE硬盘的一种接口
   drv = MMC = 1: 表示的是MMC卡的接口，也就是SD卡
   drv = USB = 2: 表示的是USB存储设备的接口
   从这里看来，我们应该要选择的是MMC接口,输入参数必须为1*/
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
  SD_Error Status;
  Status = SD_Init();
  
  if(Status == SD_OK)return 0;
  else return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status  [该函数需要改写]                                                  */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s) [该函数需要改写]                                       */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	SD_Error Status = SD_OK;

	if(count == 1 ){

	    Status = SD_ReadBlock(buff, sector << 9, 512);
#if defined (SD_DMA_MODE)
	    /* Check if the Transfer is finished */
	    Status = SD_WaitReadOperation();
	    while(SD_GetStatus() != SD_TRANSFER_OK);
#endif
	}
	else{
		
		/* Read block of many bytes from address 0 */
		Status = SD_ReadMultiBlocks(buff, sector << 9, 512, count);
#if defined (SD_DMA_MODE)
		/* Check if the Transfer is finished */
		Status = SD_WaitReadOperation();
		while(SD_GetStatus() != SD_TRANSFER_OK);
#endif
	}

	if(Status == SD_OK)
		return RES_OK;
	else 
		return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s) [该函数需要改写]                                                      */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	SD_Error Status = SD_OK;

	if(count == 1){
		/* Write block of 512 bytes on address 0 */
		Status = SD_WriteBlock((u8 *)(&buff[0]), sector << 9, 512);
#if defined (SD_DMA_MODE)
		/* Check if the Transfer is finished */
		Status = SD_WaitWriteOperation();
  	  while(SD_GetStatus() != SD_TRANSFER_OK);
#endif
	}
	else 
		{
			/* Write multiple block of many bytes on address 0 */
			Status = SD_WriteMultiBlocks((u8 *)(&buff[0]), sector << 9, 512, count);
#if defined (SD_DMA_MODE)
			/* Check if the Transfer is finished */
			Status = SD_WaitWriteOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
#endif
	}
     
	if(Status == SD_OK)
		return RES_OK;
	else 
		return RES_ERROR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions [该函数需要改写]                                              */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	return RES_OK;
}

