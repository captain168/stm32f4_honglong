#ifndef __VARIABLE_H__
#define __VARIABLE_H__
#include "typedef.h"
#include "diskio.h"
#include "ff.h"	



vu32 					TimeDisplay;
volatile SCANCTRL		gScanCtrl;
volatile SCANVALUEGROUP	gScanValueGroup;
volatile STATUS 		gSysStatus,gSysStatusBkp;
//volatile DSTATUS 		gEquipStatus.SD_status;
volatile EQUIPSTATUS	gEquipStatus;
volatile bool			SD_insert_IRQ_flag;

vu16 mvCnt ;
vu8 SDSaveRowCounter;	
    
FRESULT sdres;         // FatFs function common result code
int resTemp;
volatile char strDocName[21];


//const u8 gSpeedArray[6] = {10,20,50,100,150,200};

#endif
