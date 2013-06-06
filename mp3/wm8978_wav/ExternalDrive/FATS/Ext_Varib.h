#ifndef __EXTVARIB_H__
#define __EXTVARIB_H__
#include "diskio.h"
#include "ff.h"	


extern vu32 						TimeDisplay;
extern volatile SCANCTRL			gScanCtrl;
extern volatile SCANVALUEGROUP		gScanValueGroup;
extern volatile STATUS 				gSysStatus,gSysStatusBkp;
extern const    u8 					gSpeedArray[6];
//extern volatile DSTATUS 			gEquipStatus.SD_status;
extern volatile EQUIPSTATUS			gEquipStatus;
extern volatile bool				SD_insert_IRQ_flag;

extern vu16 mvCnt;
extern vu8 SDSaveRowCounter;	

extern FATFS fs;            	// Work area (file system object) for logical drive
extern volatile FIL fsrc;      			// file objects	    
extern FRESULT sdres;         	// FatFs function common result code
extern int resTemp;
extern volatile char strDocName[21];

#endif

