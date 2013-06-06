/******************************Copyright (c)********************************
**
**                          http://www.powermcu.com
**
**--------------File Info---------------------------------------------------
** File name:               fsmc_nand.c
** Descriptions:            The FSMC Nand application function
**
**--------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-16
** Version:                 v1.0
** Descriptions:            The original version
**
**-------------------------------------------------------------------------
** Modified by:             RTT/heyuanjie87
** Modified date:           2012-8-15
** Version:                 v1.1
** Descriptions:            Used for RealTouch NandFlash Test
**
**************************************************************************/

/* Includes -------------------------------------------------------------*/
#include "fsmc_nand.h"
#include "uart.h"

/* Private define -------------------------------------------------------*/
#define NAND_FLASH_START_ADDR     ((uint32_t)0x70000000) /* Bank3 */

#define ROW_ADDRESS (Address.Page + (Address.Block + (Address.Zone * \
                     NAND_ZONE_SIZE)) * NAND_BLOCK_SIZE)

/* Private variables -----------------------------------------------*/
static uint8_t TxBuffer [NAND_PAGE_SIZE];
static uint8_t RxBuffer [NAND_PAGE_SIZE];

/* Private function ------------------------------------------------*/
static uint32_t FSMC_NAND_AddressIncrement(NAND_ADDRESS* Address);
static void FSMC_NAND_GPIO_Init(void);


/*******************************************************************************
* Function Name  : FSMC_NAND_GPIO_Init
* Description    : Init GPIO that NandFlash needs 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/

static void FSMC_NAND_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 开启GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | \
                           RCC_AHB1Periph_GPIOG, ENABLE);

    /**** 初始化NandFlash要用到的GPIO ****/

    /* D2，D3（数据位） */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
    /* NOE, NWE */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    /* SRAM片选，NE1 */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);

    /* A16,A17 config as fsmc */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
    /* D0 D1（数据位） */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    /* 配置GPIOE */
    /* D4 D5 D6 D7（数据位） */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);

    /* 配置GPIOG */
    /* 6. FSMC_INT2 configuration */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_FSMC);
    /* NandFlash片选，NE2 */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);
		
		GPIO_PinAFConfig(GPIOG, GPIO_PinSource10 , GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | \
                                  GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_9 | \
                                  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | \
                                  GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | \
                                  GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6  | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : FSMC_NAND_Init
* Description    : Configures the FSMC and GPIOs to interface with the NAND memory.
*                  This function must be called before any write/read operation
*                  on the NAND.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/

void FSMC_NAND_Init(void)
{
    FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
    FSMC_NAND_PCCARDTimingInitTypeDef  p;

    FSMC_NAND_GPIO_Init();
 
    /**** 配置FSMC ****/

    /* 开启FSMC的时钟 */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    p.FSMC_SetupTime     = 0x1;
    p.FSMC_WaitSetupTime = 0x3;
    p.FSMC_HoldSetupTime = 0x2;
    p.FSMC_HiZSetupTime  = 0x1;

    FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
    FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;
    FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Disable;
    FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
    FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
    FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

    FSMC_NANDInit(&FSMC_NANDInitStructure);

    /* FSMC NAND Bank Cmd Test */
    FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
}

/******************************************************************************
* Function Name  : FSMC_NAND_ReadID
* Description    : Reads NAND memory's ID.
* Input          : - NAND_ID: pointer to a NAND_IDTypeDef structure which will hold
*                    the Manufacturer and Device ID.
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FSMC_NAND_ReadID(NAND_IDTypeDef* NAND_ID)
{
    /* Send Command to the command area */
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READID;
	/* Send Address to the address area */
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;

    /* Sequence to read ID from NAND flash */
    NAND_ID->Maker_ID   = *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA); 
    NAND_ID->Device_ID  = *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA); 
    NAND_ID->Third_ID   = *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA); 
    NAND_ID->Fourth_ID  = *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA); 
    NAND_ID->Fifth_ID   = *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA);
}

/******************************************************************************
* Function Name  : FSMC_NAND_WritePage
* Description    : This routine is for writing one or several 512 Bytes Page size.
* Input          : - pBuffer: pointer on the Buffer containing data to be written
*                  - Address: First page address
*                  - NumPageToWrite: Number of page to write
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_WritePage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToWrite)
{
    uint32_t index = 0x00, numpagewritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
    uint32_t status = NAND_READY, size = 0x00;

    while((NumPageToWrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
    {
        /* Page write command and address */
        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0X00;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
#ifdef K9F2G08
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
#endif

        /* Calculate the size */
        size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpagewritten);

        /* Write data */
        for(; index < size; index++)
        {
            *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA) = pBuffer[index];
        }

        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;

        while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );

        /* Check status for successful operation */
        status = FSMC_NAND_GetStatus();

        if(status == NAND_READY)
        {
            numpagewritten++;

            NumPageToWrite--;

            /* Calculate Next page Address */
            addressstatus = FSMC_NAND_AddressIncrement(&Address);
        }
    }

    return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_ReadPage
* Description    : This routine is for sequential read from one or several
*                  512 Bytes Page size.
* Input          : - pBuffer: pointer on the Buffer to fill
*                  - Address: First page address
*                  - NumPageToRead: Number of page to read
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_ReadPage(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumPageToRead)
{
    uint32_t index = 0x00, numpageread = 0x00, addressstatus = NAND_VALID_ADDRESS;
    uint32_t status = NAND_READY, size = 0x00;

    while((NumPageToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
    {
        /* Page Read command and page address */
        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_1;

        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0X00;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
#ifdef K9F2G08
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
#endif

        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE;

        while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );

        /* Calculate the size */
        size = NAND_PAGE_SIZE + (NAND_PAGE_SIZE * numpageread);

        /* Get Data into Buffer */
        for(; index < size; index++)
        {
            pBuffer[index]= *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA);
        }

        numpageread++;

        NumPageToRead--;

        /* Calculate page address */
        addressstatus = FSMC_NAND_AddressIncrement(&Address);
    }

    status = FSMC_NAND_GetStatus();

    return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_WriteSpareArea
* Description    : This routine write the spare area information for the specified
*                  pages addresses.
* Input          : - pBuffer: pointer on the Buffer containing data to be written
*                  - Address: First page address
*                  - NumSpareAreaTowrite: Number of Spare Area to write
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_WriteSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaTowrite)
{
    uint32_t index = 0x00, numsparesreawritten = 0x00, addressstatus = NAND_VALID_ADDRESS;
    uint32_t status = NAND_READY, size = 0x00;

    while((NumSpareAreaTowrite != 0x00) && (addressstatus == NAND_VALID_ADDRESS) && (status == NAND_READY))
    {
        /* Page write Spare area command and address */
        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM;

        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x08;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
#ifdef K9F2G08
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
#endif

        /* Calculate the size */
        size = NAND_SPARE_AREA_SIZE + (NAND_SPARE_AREA_SIZE * numsparesreawritten);

        /* Write the data */
        for(; index < size; index++)
        {
            *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA) = pBuffer[index];
        }

        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_PAGEPROGRAM_TRUE;

        while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );

        /* Check status for successful operation */
        status = FSMC_NAND_GetStatus();

        if(status == NAND_READY)
        {
            numsparesreawritten++;

            NumSpareAreaTowrite--;

            /* Calculate Next page Address */
            addressstatus = FSMC_NAND_AddressIncrement(&Address);
        }
    }

    return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_ReadSpareArea
* Description    : This routine read the spare area information from the specified
*                  pages addresses.
* Input          : - pBuffer: pointer on the Buffer to fill
*                  - Address: First page address
*                  - NumSpareAreaToRead: Number of Spare Area to read
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
*                  And the new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_ReadSpareArea(uint8_t *pBuffer, NAND_ADDRESS Address, uint32_t NumSpareAreaToRead)
{
    uint32_t numsparearearead = 0x00, index = 0x00, addressstatus = NAND_VALID_ADDRESS;
    uint32_t status = NAND_READY, size = 0x00;

    while((NumSpareAreaToRead != 0x0) && (addressstatus == NAND_VALID_ADDRESS))
    {
        /* Page Read command and page address */
        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_1;

        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x00;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = 0x08;
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);
#ifdef K9F2G08
        *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
#endif

        *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_READ_TRUE;

        /* 读忙脚 */
        while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );

        /* Data Read */
        size = NAND_SPARE_AREA_SIZE +  (NAND_SPARE_AREA_SIZE * numsparearearead);

        /* Get Data into Buffer */
        for ( ; index < size; index++)
        {
            pBuffer[index] = *(vu8 *)(NAND_FLASH_START_ADDR | DATA_AREA);
        }

        numsparearearead++;

        NumSpareAreaToRead--;

        /* Calculate page address */
        addressstatus = FSMC_NAND_AddressIncrement(&Address);
    }

    status = FSMC_NAND_GetStatus();

    return (status | addressstatus);
}

/******************************************************************************
* Function Name  : FSMC_NAND_EraseBlock
* Description    : This routine erase complete block from NAND FLASH
* Input          : - Address: Any address into block to be erased
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_EraseBlock(NAND_ADDRESS Address)
{
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_ERASE0;

    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_1st_CYCLE(ROW_ADDRESS);
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_2nd_CYCLE(ROW_ADDRESS);

#ifdef K9F2G08
    *(vu8 *)(NAND_FLASH_START_ADDR | ADDR_AREA) = ADDR_3rd_CYCLE(ROW_ADDRESS);
#endif

    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_ERASE1;


    while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 );

    return (FSMC_NAND_GetStatus());
}

/******************************************************************************
* Function Name  : FSMC_NAND_Reset
* Description    : This routine reset the NAND FLASH
* Input          : None
* Output         : None
* Return         : NAND_READY
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_Reset(void)
{
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_RESET;

    return (NAND_READY);
}

/******************************************************************************
* Function Name  : FSMC_NAND_GetStatus
* Description    : Get the NAND operation status
* Input          : None
* Output         : None
* Return         : New status of the NAND operation. This parameter can be:
*                   - NAND_TIMEOUT_ERROR: when the previous operation generate
*                     a Timeout error
*                   - NAND_READY: when memory is ready for the next operation
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_GetStatus(void)
{
    uint32_t timeout = 0x1000000, status = NAND_READY;

    status = FSMC_NAND_ReadStatus();

    /* Wait for a NAND operation to complete or a TIMEOUT to occur */
    while ((status != NAND_READY) &&( timeout != 0x00))
    {
        status = FSMC_NAND_ReadStatus();
        timeout --;
    }

    if(timeout == 0x00)
    {
        status =  NAND_TIMEOUT_ERROR;
    }

    /* Return the operation status */
    return (status);
}
/******************************************************************************
* Function Name  : FSMC_NAND_ReadStatus
* Description    : Reads the NAND memory status using the Read status command
* Input          : None
* Output         : None
* Return         : The status of the NAND memory. This parameter can be:
*                   - NAND_BUSY: when memory is busy
*                   - NAND_READY: when memory is ready for the next operation
*                   - NAND_ERROR: when the previous operation gererates error
* Attention		 : None
*******************************************************************************/
uint32_t FSMC_NAND_ReadStatus(void)
{
    uint32_t data = 0x00, status = NAND_BUSY;

    /* Read status operation ------------------------------------ */
    *(vu8 *)(NAND_FLASH_START_ADDR | CMD_AREA) = NAND_CMD_STATUS;
    data = *(vu8 *)(NAND_FLASH_START_ADDR);

    if((data & NAND_ERROR) == NAND_ERROR)
    {
        status = NAND_ERROR;
    }
    else if((data & NAND_READY) == NAND_READY)
    {
        status = NAND_READY;
    }
    else
    {
        status = NAND_BUSY;
    }

    return (status);
}

/******************************************************************************
* Function Name  : NAND_AddressIncrement
* Description    : Increment the NAND memory address
* Input          : - Address: address to be incremented.
* Output         : None
* Return         : The new status of the increment address operation. It can be:
*                  - NAND_VALID_ADDRESS: When the new address is valid address
*                  - NAND_INVALID_ADDRESS: When the new address is invalid address
* Attention		 : None
*******************************************************************************/
static uint32_t FSMC_NAND_AddressIncrement(NAND_ADDRESS* Address)
{
    uint32_t status = NAND_VALID_ADDRESS;

    Address->Page++;

    if(Address->Page == NAND_BLOCK_SIZE)
    {
        Address->Page = 0;
        Address->Block++;

        if(Address->Block == NAND_ZONE_SIZE)
        {
            Address->Block = 0;
            Address->Zone++;

            if(Address->Zone == NAND_MAX_ZONE)
            {
                status = NAND_INVALID_ADDRESS;
            }
        }
    }

    return (status);
}

/******************************************************************************
* Function Name  : FSMC_NAND_Test
* Description    : NAND test
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void FSMC_NAND_Test(void)
{
    uint16_t index;
    NAND_IDTypeDef NAND_ID;
    NAND_ADDRESS WriteReadAddr;

    FSMC_NAND_Init();

    /* 检测NAND Flash */
    FSMC_NAND_ReadID(&NAND_ID);
    debug("Nand Flash ID = %02X,%02X\n\r",NAND_ID.Maker_ID, NAND_ID.Device_ID);

    if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xF1)
            && (NAND_ID.Third_ID == 0x80) && (NAND_ID.Fourth_ID == 0x15))
    {
        debug("Type = K9F1G08U0A\r\n");
    }
    else if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xF1)
             && (NAND_ID.Third_ID == 0x00) && (NAND_ID.Fourth_ID == 0x95))
    {
        debug("Type = K9F1G08U0B\r\n");
    }
    else if ((NAND_ID.Maker_ID == 0xAD) && (NAND_ID.Device_ID == 0xF1)
             && (NAND_ID.Third_ID == 0x80) && (NAND_ID.Fourth_ID == 0x1D))
    {
        debug("Type = HY27UF081G2A\r\n");
    }
    else if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xDA)
             && (NAND_ID.Third_ID == 0x10) && (NAND_ID.Fourth_ID == 0x95))
    {
        debug("Type = K9F2G08U0A\r\n");
    }
    else if ((NAND_ID.Maker_ID == 0xEC) && (NAND_ID.Device_ID == 0xAA)
             && (NAND_ID.Third_ID == 0x00) && (NAND_ID.Fourth_ID == 0x15))
    {
        debug("Type = K9F2G08R0A\r\n");
    }
    else
    {
        debug("Type = Unknow\r\n");
    }
    
}
 
/*********************************************************************
      END FILE

*********************************************************************/
