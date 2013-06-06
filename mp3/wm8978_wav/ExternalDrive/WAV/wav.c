#include "wav.h"
#include "i2s.h"
//#include "usart.h"
#include "stm32f4xx.h"
#include "bsp_wm8978.h"


/*SD 卡需要的头文件*/
#include "sdio_sd.h"
#include "ff.h"
#include "ffconf.h"

#include <stdio.h>

/*关于SD卡的申明*/
extern SD_CardInfo SDCardInfo;
DIR dirs;
FILINFO finfo;
FATFS fs;


/////////////////////////////
WAVE Wave;	//wave重要的3个要素



/*
	处理WAV的文件头
*/
//http://blog.sina.com.cn/s/blog_6a25068b0100lv45.html
void WAVE_HeardInfo(unsigned char* buf,WAVE_HEADER* wave_heard)
{
	u8 base;
	wave_heard->RIFF.szRiffID[0] = buf[0];
	wave_heard->RIFF.szRiffID[1] = buf[1];
	wave_heard->RIFF.szRiffID[2] = buf[2];
	wave_heard->RIFF.szRiffID[3] = buf[3];
	wave_heard->RIFF.dwRiffSize  = buf[4]+(buf[5]<<8)+(buf[6]<<16)+(buf[7]<<24);
	wave_heard->RIFF.szRiffFormat[0] = buf[8];
	wave_heard->RIFF.szRiffFormat[1] = buf[9];
	wave_heard->RIFF.szRiffFormat[2] = buf[10];
	wave_heard->RIFF.szRiffFormat[3] = buf[11];
//	printf("wav_head.RIFF.szRiffID=%s\r\n",&wave_heard.RIFF.szRiffFormat[0]);

	wave_heard->RMT.szFmtID[0] = buf[12];
	wave_heard->RMT.szFmtID[1] = buf[13];
	wave_heard->RMT.szFmtID[2] = buf[14];
	wave_heard->RMT.szFmtID[3] = buf[15];
	wave_heard->RMT.dwFmtSize  = buf[16]+(buf[17]<<8)+(buf[18]<<16)+(buf[19]<<24);

	wave_heard->RMT.wavFormat.wFormatTag = buf[20]+(buf[21]<<8);
	wave_heard->RMT.wavFormat.wChannels	= buf[22]+(buf[23]<<8);
	wave_heard->RMT.wavFormat.dwSamplesPerSec = buf[24]+(buf[25]<<8)+(buf[26]<<16)+(buf[27]<<24);
	wave_heard->RMT.wavFormat.dwAvgBytesPerSec = buf[28]+(buf[29]<<8)+(buf[30]<<16)+(buf[31]<<24);
	wave_heard->RMT.wavFormat.wBlockAlign = buf[32]+(buf[33]<<8);
	wave_heard->RMT.wavFormat.wBitsPerSample = buf[34]+(buf[35]<<8);

	//占时不考虑FACT块
	//判断有无附加块
	if(wave_heard->RMT.dwFmtSize == 16){
		
		base = 36;
	}
	else if(wave_heard->RMT.dwFmtSize == 18) {
	
		base = 38;
	}

	wave_heard->DATA.szDataID[0] = buf[base+0];
	wave_heard->DATA.szDataID[1] = buf[base+1];
	wave_heard->DATA.szDataID[2] = buf[base+2];
	wave_heard->DATA.szDataID[3] = buf[base+3];
	wave_heard->DATA.dwDataSize = buf[base+4]
			+(buf[base+5]<<8)+(buf[base+6]<<16)+(buf[base+7]<<24);



	//重要是
	Wave.Channels = buf[22]+(buf[23]<<8);//通道
	Wave.SamplesPerSec = buf[24]+(buf[25]<<8)+(buf[26]<<16)+(buf[27]<<24);	//采样频率
	Wave.BitsPerSample = buf[34]+(buf[35]<<8);//每秒采样位数

	printf("通道：0x%x\r\n",Wave.Channels);
	printf("频率：0x%x\r\n",Wave.SamplesPerSec);
	printf("位数：0x%x\r\n",Wave.BitsPerSample);


}



/*
 	测试函数
*/
u8 buf[512],buf2[512];
u16 wav_count=0,wav_count2=512;
u8 select_buf;
extern unsigned char *addr;
void WAVE_Test()
{
	u16 i;
	

	WAVE_HEADER wav_head;

	UINT br;
	FRESULT res;
	FIL fsrc;
//
//	wm8978_WriteReg(3,0xff|(1<<8)|(1)|(1<<2));
//	wm8978_WriteReg(1,(1<<2)|(1<<8)|(1<<7)|(1<<3)|(1)|(0<<5));
//	wm8978_WriteReg(2,(1<<7)|(1<<8)|(1<<4)|(1<<5)|(1<<2)|(1<<3));
//	wm8978_WriteReg(4,/*(1<<7)|(1<<8)|*/(2<<3)|(1<<2));
//	wm8978_WriteReg(6,0x00);
//	wm8978_WriteReg(10,1<<3);
//	wm8978_WriteReg(43,1<<4);       

	wm8978_WriteReg(3,0xff|(1<<8)|(1)|(1<<2));
	wm8978_WriteReg(1,(1<<2)|(1<<8)|(1<<7)|(1<<3)|(1));
	wm8978_WriteReg(2,(1<<7)|(1<<8)|(1<<4)|(1<<5)|(1<<2)|(1<<3));
	wm8978_WriteReg(4,/*(1<<7)|(1<<8)|*/(2<<3)|(1<<2));
	wm8978_WriteReg(6,0);
	wm8978_WriteReg(10,1<<3);
	wm8978_WriteReg(43,1<<4);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               

//	SPI_I2S_SendData(SPI3,i);
//	SysTick_Config(0xffff);

//	wm8978_CfgAudioIF(I2S_Standard_PCMLong, 16, I2S_Mode_MasterRx);
//	I2S_Mode_Config(I2S_Standard_PCMShort,I2S_DataFormat_16b,I2S_AudioFreq_22k,I2S_Mode_MasterTx);


	/*挂载文件系统*/
	f_mount(0, &fs);
	
//	while(1){
								  	
		res = f_open(&fsrc, "0:1.wav", FA_OPEN_EXISTING | FA_READ);
		printf("open  res=0x%x\r\n",res);

		/*读文件到缓冲（）*/
		res = f_read(&fsrc, buf, 62, &br);	 /*因为这是通过工具进行的格式转换所以有62*/
		/**/
		WAVE_HeardInfo((u8*)buf,&wav_head);
		wav_count = 512;
		while(1){
		
			res = f_read(&fsrc, buf, 512, &br);
			if(res !=0){
				while(1)
				printf("error->fread\r\n");
			}
			while(wav_count<255);
			SPI_I2S_ITConfig(I2Sx,SPI_I2S_IT_TXE,DISABLE);
			wav_count=0;
			addr = buf;
			//select_buf=1;
			select_buf=2;
			SPI_I2S_ITConfig(I2Sx,SPI_I2S_IT_TXE,ENABLE);

//			res = f_read(&fsrc, buf2, 512, &br);
//			if(res !=0){
//				while(1)
//				printf("error->fread\r\n");
//			}
//			while(wav_count<255);
//			SPI_I2S_ITConfig(I2Sx,SPI_I2S_IT_TXE,DISABLE);
//			wav_count=0;
//			addr = buf2;
//			//select_buf=1;
//			select_buf=2;
//			SPI_I2S_ITConfig(I2Sx,SPI_I2S_IT_TXE,ENABLE);

			if(br<512){
				SPI_I2S_ITConfig(I2Sx,SPI_I2S_IT_TXE,DISABLE);
				printf("播放完毕\r\n");
				f_close(&fsrc);
				break;
			}	
		}
	
//	}
	

}


