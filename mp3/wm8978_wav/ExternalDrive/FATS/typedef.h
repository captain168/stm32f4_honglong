#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__
#include "diskio.h"

typedef enum _POWERS { LiLion = 0,ExLion } POWERS;	 // 锂电or 外置电源
typedef struct _SCANCTRL
{
	u16  ScanSpeedTimerBkp;			// backup timer2's count initialization 
	u16  VoltaRange;		// be used for scanning voltage range control(DAC output range)
	u16  NowDac;				// the DAC output value when time2 run	
	s16  ScanDir;				// the DAC scan direction，up or down when time2 run	
	u8   Max326Range;		// the amplifier's magnifying  multiple（for set max326）
	vu16   mv_OnePixel_X;	// how many mv voltage equal one x-axis lcd pixel
	u8   mv_OnePixel_Y;	// how many mv voltage equal one y-axis lcd pixel	
	u8   ScanSpeed;			// be used for scan speed control
	u8   SpeedIndex;		// SpeedIndex(速度放在一个数组，通过这个索引将速度值放入Speed) 
	u8   CursorLocation;// used for indicating Cursor in which Edit when setting	parameter
	bool OneCircleMark; // indicate DAC output value has scan One Circle
	bool OnePointMark; // indicate DAC output value has scan One Circle		
	FlagStatus OK_KeyMark;		// A mark for whether OK_Key pressed or not	
	u16  Scan_Times;
} SCANCTRL;

typedef struct _SCANVALUE
{
	s16 DACValue;
	s32 ADCValue;
}	SCANVALUE;

typedef struct _EQUIPSTATUS
{
	volatile DSTATUS SD_status;	
	volatile bool	 Power_Source;
	volatile u8		 LiLion_Quantity;	
	volatile bool	 RTC_status;
}	EQUIPSTATUS;

typedef struct _SCANVALUEGROUP
{
	SCANVALUE ScanValue[400];
	u16  VoltaRange;
	vu16 ArrayIn;
	vu16 ArrayOut;
	u16	 ScanedGroupCnt;
	u8   ScanSpeed;	
	u32	 ScanedTime;	// from shart to now(now this wave's starting time ,is the sample test starting to this wave time)
	u32  ScanRealTime;	// the rtc time

}SCANVALUEGROUP;

typedef enum _STATUS { Idle = 0,SETPARA,SELECT,RUN,STOP } STATUS;

#endif
