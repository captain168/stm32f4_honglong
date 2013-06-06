#include "i2s.h"
#include "stm32f4xx.h"
#include "uart.h"
#include "mad.h"


/**
  * @brief  Configures the I2S Peripheral.
  * @param  None
  * @retval None
  */

void wm8978_I2S_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2S_InitTypeDef I2S_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  extern unsigned short *addr;
  extern const unsigned int INPUT_BUFFER_SIZE ;//(5*8192)
  extern const unsigned int OUTPUT_BUFFER_SIZE ;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the I2Sx/I2Sx_ext clock */
  I2Sx_CLK_INIT(I2Sx_CLK, ENABLE);
  
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(I2Sx_WS_GPIO_CLK | I2Sx_CK_GPIO_CLK | \
                         I2Sx_SD_GPIO_CLK , ENABLE);

  /* I2S GPIO Configuration --------------------------------------------------*/
  /* Connect I2S pins to Alternate functions */  
  GPIO_PinAFConfig(I2Sx_WS_GPIO_PORT, I2Sx_WS_SOURCE, I2Sx_WS_AF);
  GPIO_PinAFConfig(I2Sx_CK_GPIO_PORT, I2Sx_CK_SOURCE, I2Sx_CK_AF);
  GPIO_PinAFConfig(I2Sx_SD_GPIO_PORT, I2Sx_SD_SOURCE, I2Sx_SD_AF);
  GPIO_PinAFConfig(I2Sx_MCLK_GPIO_PORT, I2Sx_MCLK_SOURCE, I2Sx_MCLK_AF);


  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /* I2S WS pin configuration */
  GPIO_InitStructure.GPIO_Pin = I2Sx_WS_PIN;
  GPIO_Init(I2Sx_WS_GPIO_PORT, &GPIO_InitStructure);

  /* I2S CK pin configuration */
  GPIO_InitStructure.GPIO_Pin =  I2Sx_CK_PIN;
  GPIO_Init(I2Sx_CK_GPIO_PORT, &GPIO_InitStructure);
  
  /* I2S SD pin configuration */
  GPIO_InitStructure.GPIO_Pin = I2Sx_SD_PIN;
  GPIO_Init(I2Sx_SD_GPIO_PORT, &GPIO_InitStructure);

  /*MCLK*/
  GPIO_InitStructure.GPIO_Pin = I2Sx_MCLK_PIN;
  GPIO_Init(I2Sx_MCLK_GPIO_PORT, &GPIO_InitStructure);


 
  /* I2S configuration -------------------------------------------------------*/
  /* Initialize  I2Sx and I2Sxext peripherals */
  SPI_I2S_DeInit(I2Sx);
  /* Configure the Audio Frequency, Standard and the data format */
  I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_11k;
  I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
  I2S_Init(I2Sx, &I2S_InitStructure);
  
  /* Configure the I2Sx_ext (the second instance) in Slave Receiver Mode */
//  I2S_FullDuplexConfig(I2Sxext, &I2S_InitStructure);

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
//    
//    /* Configure the DMA Stream */
//    DMA_Cmd(DMA1_Stream4, DISABLE);
//    DMA_DeInit(DMA1_Stream4);
//    /* Set the parameters to be configured */
//    DMA_InitStructure.DMA_Channel = DMA_Channel_3;  
//    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI3_BASE+0x0c;;
//    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)addr;     /* This field will be configured in play function */
//    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//    DMA_InitStructure.DMA_BufferSize = (uint32_t)OUTPUT_BUFFER_SIZE;      /* This field will be configured in play function */
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
//    DMA_Init(DMA1_Stream4, &DMA_InitStructure); 
//
//	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
//	DMA_Cmd(DMA1_Stream4, ENABLE);
//
//	 NVIC_InitStructure.NVIC_IRQChannel = DMA_Channel_3;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//
//
//	SPI_I2S_DMACmd(I2Sx, SPI_I2S_DMAReq_Rx, ENABLE); 
  
  /* Enable the I2Sx peripheral */
  I2S_Cmd(I2Sx, ENABLE);
  /* Enable the I2Sx_ext peripheral for Full Duplex mode */ 
 // I2S_Cmd(I2Sxext, !ENABLE);

		
	
	
	NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
////DMA传送配置
//void Audio_DMA_Init(void)  
//{ 
//  	NVIC_InitTypeDef NVIC_InitStructure;
//  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
//  	DMA_Cmd(DMA1_Stream5, DISABLE);
//  	DMA_DeInit(DMA1_Stream5);
//  	DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
//  	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&I2Sx->DR);
//  	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;//该项将在播放函数中进行配置
//  	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
//  	DMA_InitStructure.DMA_BufferSize = (uint32_t)0xFFFE;//该项将在播放函数中进行配置      
//  	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord; 
//  	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
//  	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//  	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//  	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
//  	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//  	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
//  	DMA_Init(DMA1_Stream5, &DMA_InitStructure);  
//  	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);//使能所选DMA的中断
//
//  	//I2S DMA中断通道配置
//  	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级(0 最高)
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//子优先级(0 最高)
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  	NVIC_Init(&NVIC_InitStructure);   
//  	SPI_I2S_DMACmd(I2Sx, SPI_I2S_DMAReq_Tx, ENABLE);//中断请求使能  
//}
void IIS_Config(struct mad_pcm* madpcm)
{
	I2S_InitTypeDef I2S_InitStructure;
	I2S_Cmd(I2Sx, DISABLE);
	 /* I2S configuration -------------------------------------------------------*/
	/* Initialize  I2Sx and I2Sxext peripherals */
	SPI_I2S_DeInit(I2Sx);
	/* Configure the Audio Frequency, Standard and the data format */
	I2S_InitStructure.I2S_AudioFreq = madpcm->samplerate;//I2S_AudioFreq_11k;
	I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
	
	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
	I2S_Init(I2Sx, &I2S_InitStructure);
	
	/* Enable the I2Sx peripheral */
	I2S_Cmd(I2Sx, ENABLE);


}


/*
	中断处理
*/
//u8 READ_EN=0;
//char buf[1024];
//void SPI3_IRQHandler(void)
//{
//	if(SPI_I2S_GetFlagStatus(SPI3,SPI_I2S_FLAG_TXE)){
//	
//			if(READ_EN <40)
//				SPI_I2S_SendData(SPI3,buf[READ_EN]);
//			printf("fasdfa\r\n");
//			READ_EN++;		
//	}	
//}

/*I2S测试函数*/
void I2S_Test1(void)
{
	while(1){
		SPI_I2S_SendData(SPI3,0x55);	
	}
}

