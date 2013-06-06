#include "stm32f4xx_conf.h"
#include "nandflash.h"
#include "uart.h"

#define NAND_FLASH_START_ADDR     ((uint32_t)0x80000000)

void nandflash_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
  FSMC_NAND_PCCARDTimingInitTypeDef  p;

  /**** 初始化GPIO ****/
  
  /* 开启NandFlash要用到的GPIO时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | 
                         RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

  /* 配置NandFlash的数据I/O */
  /* D0 D1 */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

  /*D2,D3*/
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);

  /* D4 D5 D6 D7 */
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
  
  /* 初始化GPIOD */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | \
	                            GPIO_Pin_14 | GPIO_Pin_15 ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* 初始化GPIOE */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | \
                                GPIO_Pin_9 | GPIO_Pin_10;						   
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* 配置NandFlash的地址I/O */
  /* A16,A17 */
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource12, GPIO_AF_FSMC);

  /* 配置NOE, NWE */
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 ;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* 配置NCE3 */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_FSMC);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;

  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* 配置NWAIT */
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource6, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* 配置FSMC_INT2 */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_FSMC);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  /**** 初始化FSMC ****/
  p.FSMC_SetupTime     = 0x1;
  p.FSMC_WaitSetupTime = 0x3;
  p.FSMC_HoldSetupTime = 0x2;
  p.FSMC_HiZSetupTime  = 0x1;

  FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank3_NAND;
  FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Enable;
  FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
  FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Enable;
  FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
  FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
  FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
  FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
  FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

  FSMC_NANDInit(&FSMC_NANDInitStructure);

  /* FSMC NAND Bank Cmd Test */
  FSMC_NANDCmd(FSMC_Bank3_NAND, ENABLE);
}

void nandflash_readid(NAND_IDTypeDef* id)
{
  uint32_t data = 0;

  /*!< Send Command to the command area */

  *(__IO uint8_t *)(NAND_FLASH_START_ADDR | CMD_AREA) = 0x90;

  *(__IO uint8_t *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;

  /*!< Sequence to read ID from NAND flash */

  data = *(__IO uint32_t *)(NAND_FLASH_START_ADDR | DATA_AREA);

  id->Maker_ID   = ADDR_1st_CYCLE (data);//四个周期读取四个ID
  id->Device_ID  = ADDR_2nd_CYCLE (data);
  id->Third_ID   = ADDR_3rd_CYCLE (data);
  id->Fourth_ID  = ADDR_4th_CYCLE (data);
}

void nandtest(void)
{
    NAND_IDTypeDef id;
	
    nandflash_init();
	nandflash_readid(&id);
	debug("ID:0x%X,0x%X\n\r",id.Maker_ID,id.Device_ID);


}

