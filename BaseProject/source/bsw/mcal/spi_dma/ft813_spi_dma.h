/*
 * ft813_spi_dma.h
 *
 *  Created on: Feb 5, 2018
 *      Author: Samuel
 */

#ifndef _FT813_SPI_DMA_H_
#define _FT813_SPI_DMA_H_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *         Defines
 *----------------------------------------------------------------------------*/

/** An unspecified error has occurred.*/
#define FT813_ERROR_DMA_ALLOCATE_CHANNEL          1
#define FT813_ERROR_DMA_CONFIGURE                 2
#define FT813_ERROR_DMA_TRANSFER                  3
#define FT813_ERROR_DMA_SIZE                      4

/*------------------------------------------------------------------------------
 *         Types Definitions
 *----------------------------------------------------------------------------*/

/** FT813 LCD Controller Type Definitions */

typedef struct _FT813_dma {
    /** Pointer to DMA driver */
    sXdmad              *xdmaD;
    /** FT813 Tx channel */
    uint32_t            FT813DmaTxChannel;
    /** FT813 Rx channel */
    uint32_t            FT813DmaRxChannel;
    /** FT813 Tx/Rx configure descriptor */
    sXdmadCfg           xdmadRxCfg, xdmadTxCfg;
    /** FT813 dma interrupt */
    uint32_t            xdmaInt;
    /** Pointer to SPI Hardware registers */
    Spi                *pSpiHw;
    /** SPI Id as defined in the product datasheet */
    uint8_t             spiId;
} sFT813Dma;

typedef struct _FT813_ctl {
    /** FT813 Command/Data mode */
    volatile uint8_t       cmdOrDataFlag;
    /** FT813 Rx done */
    volatile uint8_t       rxDoneFlag;
    /** FT813 Tx done */
    volatile uint8_t       txDoneFlag;
} sFT813DmaCtl;

typedef enum{
     AccessInst = 0,
     AccessRead,
     AccessWrite
}AccessFt_t;

void FT813_SpiDmaInitialize(sXdmad *dmad);

uint8_t _FT813_SpiDmaConfigChannels(void);

uint8_t _FT813_SpiDmaConfigureRxTx(void);

#endif /* _FT813_SPI_DMA_H_ */
