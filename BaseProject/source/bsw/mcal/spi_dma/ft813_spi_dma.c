/*
 * ft813_spi_dma.c
 *
 *  Created on: Feb 5, 2018
 *      Author: Samuel
 */


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "board.h"
#include "ft813_spi_dma.h"

/*------------------------------------------------------------------------------
 *         Defines
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *         Types Definitions
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *         Global Variables
 *----------------------------------------------------------------------------*/

static sFT813Dma FT813DmaSpiMode;
static sFT813DmaCtl  FT813DmaCtlInSpiMode;

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 * \brief FT813_SPI xDMA Rx callback
 */
static void FT813_Spi_Rx_CB(void)
{
    if (!FT813DmaCtlInSpiMode.cmdOrDataFlag) {
        FT813DmaCtlInSpiMode.rxDoneFlag = 1;
        SCB_InvalidateDCache_by_Addr((uint32_t *)FT813DmaSpiMode.xdmadRxCfg.mbr_da, FT813DmaSpiMode.xdmadRxCfg.mbr_ubc);
    memory_barrier()
}

/**
 * \brief FT813_SPI xDMA Tx callback
 */
static void FT813_Spi_Tx_CB(void)
{
    uint32_t i;

    if (FT813DmaCtlInSpiMode.cmdOrDataFlag) {
        PIO_Set(&lcd_spi_cds_pin);

        for (i = 0; i < 0xFF; i++);
            FT813DmaCtlInSpiMode.cmdOrDataFlag = 0;
    }

    FT813DmaCtlInSpiMode.txDoneFlag = 1;
}

/**
 * \brief Initializes the FT813DmaSpiMode structure and the corresponding DMA .
 * hardware.
 */
static void FT813_SpiDmaInitialize(sXdmad *dmad)
{
    FT813DmaCtlInSpiMode.cmdOrDataFlag = 1;
    FT813DmaCtlInSpiMode.rxDoneFlag = 0;
    FT813DmaCtlInSpiMode.txDoneFlag = 1;

    FT813DmaSpiMode.xdmaD = dmad;
    FT813DmaSpiMode.xdmaD->pXdmacs = XDMAC;
    FT813DmaSpiMode.FT813DmaTxChannel = 0;
    FT813DmaSpiMode.FT813DmaRxChannel = 0;
    FT813DmaSpiMode.xdmaInt = 0;
    FT813DmaSpiMode.pSpiHw = SPI0;
    FT813DmaSpiMode.spiId = ID_SPI0;
}

/**
 * \brief This function initialize the appropriate DMA channel for Rx/Tx channel
 * of SPI or SMC
 * \returns             0 if the transfer has been started successfully;
 * otherwise returns FT813_ERROR_XX is the driver is in use,
 * or FT813_ERROR_XX if the command is not valid.
 */
uint8_t _FT813_SpiDmaConfigChannels(void)
{
    uint32_t srcType, dstType;

    /* Driver initialize */
    XDMAD_Initialize(FT813DmaSpiMode.xdmaD, 0);

    XDMAD_FreeChannel(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaTxChannel);
    XDMAD_FreeChannel(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaRxChannel);

    srcType = XDMAD_TRANSFER_MEMORY;
    dstType = FT813DmaSpiMode.spiId;

    /* Allocate a DMA channel for  FT813_SPI TX. */
    FT813DmaSpiMode.FT813DmaTxChannel = XDMAD_AllocateChannel(FT813DmaSpiMode.xdmaD, srcType, dstType);

    if (FT813DmaSpiMode.FT813DmaTxChannel == XDMAD_ALLOC_FAILED)
        return FT813_ERROR_DMA_ALLOCATE_CHANNEL;

    /* Allocate a DMA channel for FT813_SPI  RX. */
    FT813DmaSpiMode.FT813DmaRxChannel = XDMAD_AllocateChannel(FT813DmaSpiMode.xdmaD, dstType, srcType);

    if (FT813DmaSpiMode.FT813DmaRxChannel == XDMAD_ALLOC_FAILED)
        return FT813_ERROR_DMA_ALLOCATE_CHANNEL;

    /* Setup callbacks for FT813_SPI RX */
    XDMAD_SetCallback(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaRxChannel, (XdmadTransferCallback)FT813_Spi_Rx_CB, &FT813DmaSpiMode);

    if (XDMAD_PrepareChannel(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaRxChannel))
        return FT813_ERROR_DMA_ALLOCATE_CHANNEL;

    /* Setup callbacks for FT813_SPI  TX (ignored) */
    XDMAD_SetCallback(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaTxChannel, (XdmadTransferCallback)FT813_Spi_Tx_CB, &FT813DmaSpiMode);

    if (XDMAD_PrepareChannel(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaTxChannel))
        return  FT813_ERROR_DMA_ALLOCATE_CHANNEL;

    /* Check if DMA IRQ is enable; if not Enable it */
    if (!(NVIC_GetActive(XDMAC_IRQn)))
    {
        NVIC_SetPriority(XDMAC_IRQn , 1);
        /* Enable interrupt  */
        NVIC_EnableIRQ(XDMAC_IRQn);
    }

    return 0;
}

/**
 * \brief Configure the SPI/SMC tx/rx DMA.
 * \returns 0 if the xDMA configuration successfully; otherwise returns
 * FT813_ERROR_XXX.
 */
uint8_t _FT813_SpiDmaConfigureRxTx(void)
{
    uint32_t txAddress, rxAddress;
    sXdmad *pXdmad;
    pXdmad = FT813DmaSpiMode.xdmaD;

    txAddress = (uint32_t)&SPI0->SPI_TDR;
    rxAddress = (uint32_t)&SPI0->SPI_RDR;

    /* Setup DMA TX channel */
    FT813DmaSpiMode.xdmadTxCfg.mbr_sa = 0;
    FT813DmaSpiMode.xdmadTxCfg.mbr_da = txAddress;
    FT813DmaSpiMode.xdmadTxCfg.mbr_ubc = 0;     //Next Descriptor View 0

    FT813DmaSpiMode.xdmadTxCfg.mbr_cfg
        = XDMAC_CC_TYPE_PER_TRAN
          | XDMAC_CC_MBSIZE_SINGLE
          | XDMAC_CC_DSYNC_MEM2PER
          | XDMAC_CC_DWIDTH_BYTE
          | XDMAC_CC_CSIZE_CHK_1
          | XDMAC_CC_SIF_AHB_IF1
          | XDMAC_CC_DIF_AHB_IF1
          | XDMAC_CC_SAM_INCREMENTED_AM
          | XDMAC_CC_DAM_FIXED_AM
          | XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber
                           (FT813DmaSpiMode.spiId, XDMAD_TRANSFER_TX));

    FT813DmaSpiMode.xdmadTxCfg.mbr_bc = 0;
    FT813DmaSpiMode.xdmadTxCfg.mbr_sus = 0;
    FT813DmaSpiMode.xdmadTxCfg.mbr_dus = 0;

    /* Setup RX DMA channel */
    FT813DmaSpiMode.xdmadRxCfg.mbr_ubc  = 0;        //Next Descriptor View 0
    FT813DmaSpiMode.xdmadRxCfg.mbr_da = 0;
    FT813DmaSpiMode.xdmadRxCfg.mbr_sa = rxAddress;

    FT813DmaSpiMode.xdmadRxCfg.mbr_cfg =
        XDMAC_CC_TYPE_PER_TRAN
        | XDMAC_CC_MBSIZE_SINGLE
        | XDMAC_CC_DSYNC_PER2MEM
        | XDMAC_CC_CSIZE_CHK_1
        | XDMAC_CC_DWIDTH_WORD
        | XDMAC_CC_SIF_AHB_IF1
        | XDMAC_CC_DIF_AHB_IF1
        | XDMAC_CC_SAM_FIXED_AM
        | XDMAC_CC_DAM_INCREMENTED_AM
        | XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber
                         (FT813DmaSpiMode.spiId, XDMAD_TRANSFER_RX));
    FT813DmaSpiMode.xdmadRxCfg.mbr_bc = 0;
    FT813DmaSpiMode.xdmadRxCfg.mbr_sus = 0;
    FT813DmaSpiMode.xdmadRxCfg.mbr_dus = 0;

    /* Put all interrupts on for non LLI list setup of DMA */
    FT813DmaSpiMode.xdmaInt =  (XDMAC_CIE_BIE
                                  | XDMAC_CIE_RBIE
                                  | XDMAC_CIE_WBIE
                                  | XDMAC_CIE_ROIE);

    if (XDMAD_ConfigureTransfer(pXdmad, FT813DmaSpiMode.FT813DmaRxChannel, &FT813DmaSpiMode.xdmadRxCfg, 0, 0, FT813DmaSpiMode.xdmaInt))
        return FT813_ERROR_DMA_CONFIGURE;

    if (XDMAD_ConfigureTransfer(pXdmad, FT813DmaSpiMode.FT813DmaTxChannel, &FT813DmaSpiMode.xdmadTxCfg, 0, 0, FT813DmaSpiMode.xdmaInt))
        return FT813_ERROR_DMA_CONFIGURE;

    return 0;
}

/**
 * \brief Update Rx/Tx DMA configuration with new buffer address and buffer size.
 * \param pTxBuffer point to Tx buffer address
 * \param wTxSize  Tx buffer size in byte
 * \param pRxBuffer point to Rx buffer address
 * \param wRxSize Rx buffer size in byte
 * \returns 0 if the xDMA configuration successfully; otherwise returns
 * FT813_DMA_ERROR_XXX.
 */
uint8_t _FT813_SpiDmaUpdateBuffer(uint8_t *pTxBuffer, uint32_t wTxSize, uint32_t *pRxBuffer, uint32_t wRxSize)
{
    sXdmad *pXdmad;
    pXdmad = FT813DmaSpiMode.xdmaD;

    FT813DmaSpiMode.xdmadTxCfg.mbr_sa = (uint32_t)pTxBuffer;
    FT813DmaSpiMode.xdmadTxCfg.mbr_ubc = wTxSize;

    if (wRxSize) {
        FT813DmaSpiMode.xdmadRxCfg.mbr_da = (uint32_t)pRxBuffer;
        FT813DmaSpiMode.xdmadRxCfg.mbr_ubc = wRxSize;

        if (XDMAD_ConfigureTransfer(pXdmad, FT813DmaSpiMode.FT813DmaRxChannel, &FT813DmaSpiMode.xdmadRxCfg, 0, 0, FT813DmaSpiMode.xdmaInt))
            return FT813_ERROR_DMA_CONFIGURE;
    }

    if (XDMAD_ConfigureTransfer(pXdmad, FT813DmaSpiMode.FT813DmaTxChannel, &FT813DmaSpiMode.xdmadTxCfg, 0, 0, FT813DmaSpiMode.xdmaInt))
        return FT813_ERROR_DMA_CONFIGURE;

    return 0;
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/**
 * \brief Initialize FT813 driver with DMA support.
 * \returns 0 if the xDMA configuration successfully; otherwise returns
 * FT813_DMA_ERROR_XXX.
 */
uint8_t FT813_SpiInitializeWithDma(sXdmad *dmad)
{
    FT813_SpiDmaInitialize(dmad);

    if (_FT813_SpiDmaConfigChannels()) return FT813_ERROR_DMA_ALLOCATE_CHANNEL;

    if (_FT813_SpiDmaConfigureRxTx()) return FT813_ERROR_DMA_CONFIGURE;

    return 0;
}

/**
 * \brief Start FT813 DMA transfer .
 * \param pTxBuffer point to Tx buffer address
 * \param wTxSize  Tx buffer size in byte
 * \returns 0 if the xDMA configuration successfully; otherwise returns
 * FT813_DMA_ERROR_XXX.
 */
uint8_t FT813_SpiDmaTxTransfer(uint8_t *pTxBuffer, uint32_t wTxSize)
{
    while (!FT813DmaCtlInSpiMode.txDoneFlag);

    _FT813_SpiDmaUpdateBuffer(pTxBuffer, wTxSize, 0, 0);
    //SCB_CleanDCache_by_Addr((uint32_t *)pTxBuffer, wTxSize);
    SCB_CleanInvalidateDCache();
    FT813DmaCtlInSpiMode.txDoneFlag = 0;

    if (XDMAD_StartTransfer(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaTxChannel))
        return FT813_ERROR_DMA_TRANSFER;

    while (!FT813DmaCtlInSpiMode.txDoneFlag);

    return 0;
}

/**
 * \brief Start FT813 DMA Rx transfer .
 * \param pRxBuffer point to Rx buffer address
 * \param wRxSize Rx buffer size in byte
 * \returns 0 if the xDMA transfer successfully; otherwise returns
 * FT813_DMA_ERROR_XXX.
 */
uint8_t FT813_SpiDmaRxTransfer(uint32_t *pRxBuffer, uint32_t wRxSize)
{
    _FT813_SpiDmaUpdateBuffer((uint8_t *)pRxBuffer, wRxSize, (uint32_t *)pRxBuffer, wRxSize);
    //SCB_CleanDCache_by_Addr((uint32_t *)pRxBuffer, wRxSize);
    SCB_CleanInvalidateDCache();

    if (XDMAD_StartTransfer(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaRxChannel))
        return FT813_ERROR_DMA_TRANSFER;

    if (XDMAD_StartTransfer(FT813DmaSpiMode.xdmaD, FT813DmaSpiMode.FT813DmaTxChannel))
        return FT813_ERROR_DMA_TRANSFER;

    return 0;
}

/**
 * \brief Start FT813 DMA Rx transfer .
 * \param Instr Instruct
 * \param pTxData point to Tx buffer address
 * \param pRxData point to Rx buffer address
 * \param ReadWrite Command/Write/Read access
 * \param Size buffer size in byte
 * \returns 0
 */
uint8_t FT813_SpiSendCommand(uint8_t Instr, uint8_t *pTxData, uint32_t *pRxData, AccessIli_t ReadWrite, uint32_t size)
{
    if (ReadWrite == AccessInst)
    {
        PIO_Clear(&lcd_spi_cds_pin);
        FT813DmaCtlInSpiMode.cmdOrDataFlag = 1;
        return FT813_SpiDmaTxTransfer(&Instr, 1);
    }
    else if (ReadWrite == AccessWrite) {
        PIO_Clear(&lcd_spi_cds_pin);
        FT813DmaCtlInSpiMode.cmdOrDataFlag = 1;
        FT813_SpiDmaTxTransfer(&Instr, 1);

        if (size == 0) return 0;

        FT813_SpiDmaTxTransfer(pTxData, size);
        return 0;
    }
    else {
        FT813DmaCtlInSpiMode.rxDoneFlag = 0;
        FT813_SpiDmaRxTransfer(pRxData, size);

        while (!FT813DmaCtlInSpiMode.rxDoneFlag);
    }

    return 0;
}



