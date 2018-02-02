/* ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/** \addtogroup spi_module Working with SPI
 * The SPI driver provides the interface to configure and use the SPI
 * peripheral.
 *
 * The Serial Peripheral Interface (SPI) circuit is a synchronous serial
 * data link that provides communication with external devices in Master
 * or Slave Mode.
 *
 * To use the SPI, the user has to follow these few steps:
 * -# Enable the SPI pins required by the application (see pio.h).
 * -# Configure the SPI using the \ref SPI_Configure(). This enables the
 *    peripheral clock. The mode register is loaded with the given value.
 * -# Configure all the necessary chip selects with \ref SPI_ConfigureNPCS().
 * -# Enable the SPI by calling \ref SPI_Enable().
 * -# Send/receive data using \ref SPI_Write() and \ref SPI_Read(). Note that 
 * \ref SPI_Read()
 *    must be called after \ref SPI_Write() to retrieve the last value read. 
 * -# Disable the SPI by calling \ref SPI_Disable().
 *
 * For more accurate information, please look at the SPI section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref spi.c\n
 * \ref spi.h.\n
 */
/*@{*/
/*@}*/

/**
 * \file
 *
 * Implementation of Serial Peripheral Interface (SPI) controller.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

//#include "chip.h"

#include "board.h"
#include "spi.h"
#include <stdint.h>

/*------------------------------------------------------------------------------
 *         Defines
 *----------------------------------------------------------------------------*/
#define SPI0_CS3  3
#define WPEN      1     //Write protection enable
#define WPDIS     0

/*------------------------------------------------------------------------------
 *         Types Definitions
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 *         Global Variables
 *----------------------------------------------------------------------------*/

/** Pins to configure for the application. */
static const Pin SPI_PinsId[] = {
    PIN_SPI_MISO,
    PIN_SPI_MOSI,
    PIN_SPI_SPCK,
    PIN_SPI_NPCS3
};

/** SPI Clock setting (Hz) */
static uint32_t spiClock = 3000000; //3Mhz in the example, firt test with whis freq

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Initializes SPI0 peripheral.
 *
 * \param spi  Pointer to an SPI instance.
 */
extern void SPI_Init()
{
    /* Configure SPI pins*/
    PIO_Configure( SPI_PinsId, PIO_LISTSIZE(SPI_PinsId) );

    SPI_Configure(SPI0, ID_SPI0, ( SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_PCS( SPI0_CS3 )));

    /*
     *  Transmit occurs on transition from active to idle clock state (SPI_CSR_NCPHA)
     *  8 bits for transfer (SPI_CSR_BITS_8_BIT)
     */
    SPI_ConfigureNPCS( SPI0, SPI0_CS3,
                       SPI_CSR_NCPHA | SPI_CSR_BITS_8_BIT |
                       SPI_DLYBCT( 500, BOARD_MCK ) |   //Time between transfers
                       SPI_DLYBS(500, BOARD_MCK) |      //Delay Before SPCK
                       SPI_SCBR( spiClock, BOARD_MCK) );

    /* Enable SPI1 after configuration */
    SPI_Enable(SPI0);
}


//extern void SPIDma_Init()
//{
//    /* Configure SPI pins*/
//    PIO_Configure( SPI_PinsId, PIO_LISTSIZE(SPI_PinsId) );
//
//Dma.pXdmacs = XDMAC;
//
//SpiCommand.TxSize = 30;
//SpiCommand.pTxBuff = (uint8_t *)pTxBuffer;
//SpiCommand.RxSize= 30;
//SpiCommand.pRxBuff = (uint8_t *)pRxBuffer;
//SpiCommand.spiCs = SPI0_CS3;
//
//
//    SPID_Configure(&SpiDma, SPI0, ID_SPI0, (SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_PCS( SPI0_CS3 )), &Dma);
//
/*
//     *  Transmit occurs on transition from active to idle clock state (SPI_CSR_NCPHA)
//     *  8 bits for transfer (SPI_CSR_BITS_8_BIT)
//     */
//    SPI_ConfigureNPCS( SPI0, SPI0_CS3,
//                       SPI_CSR_NCPHA | SPI_CSR_BITS_8_BIT |
//                       SPI_DLYBCT( 500, BOARD_MCK ) |   //Time between transfers
//                       SPI_DLYBS(500, BOARD_MCK) |      //Delay Before SPCK
//                       SPI_SCBR( spiClock, BOARD_MCK) );
//
//    /* Enable SPI1 after configuration */
//    SPI_Enable(SPI0);
//    SPID_SendCommand(&SpiDma, &SpiCommand); //SPI_Write
//}


/*
 * SPI Send and Receive
 */
extern uint8_t SPI_ReadWrite(uint8_t DataToWrite)
{
    uint32_t DataRead = 0;

    SPI_Write(SPI0, SPI0_CS3 , (uint16_t)DataToWrite);  // Write data to SPI data register
    DataRead = SPI_Read(SPI0);

    return (uint8_t)DataRead;
}

/**
 * \brief Enables a SPI peripheral.
 *
 * \param spi  Pointer to an SPI instance.
 */
extern void SPI_Enable( Spi* spi )
{
	spi->SPI_CR = SPI_CR_SPIEN ;
}

/**
 * \brief Disables a SPI peripheral.
 *
 * \param spi  Pointer to an SPI instance.
 */
extern void SPI_Disable( Spi* spi )
{
	spi->SPI_CR = SPI_CR_SPIDIS ;
}

/**
 * \brief Enables one or more interrupt sources of a SPI peripheral.
 *
 * \param spi  Pointer to an SPI instance.
 * \param sources Bitwise OR of selected interrupt sources.
 */
extern void SPI_EnableIt( Spi* spi, uint32_t dwSources )
{
	spi->SPI_IER = dwSources ;
}

/**
 * \brief Disables one or more interrupt sources of a SPI peripheral.
 *
 * \param spi  Pointer to an SPI instance.
 * \param sources Bitwise OR of selected interrupt sources.
 */
extern void SPI_DisableIt( Spi* spi, uint32_t dwSources )
{
	spi->SPI_IDR = dwSources ;
}

/**
 * \brief Configures a SPI peripheral as specified. The configuration can be 
 * computed using several macros (see \ref spi_configuration_macros).
 *
 * \param spi  Pointer to an SPI instance.
 * \param id   Peripheral ID of the SPI.
 * \param configuration  Value of the SPI configuration register.
 */
extern void SPI_Configure( Spi* spi, uint32_t dwId, uint32_t dwConfiguration )
{
	PMC_EnablePeripheral( dwId ) ;

	spi->SPI_CR = SPI_CR_SPIDIS ;

	/* Execute a software reset of the SPI twice */
	spi->SPI_CR = SPI_CR_SWRST ;
	spi->SPI_CR = SPI_CR_SWRST ;
	spi->SPI_MR = dwConfiguration ;
}

/**
 * \brief Configures SPI chip select.
 *
 * \param spi  Pointer to an SPI instance.
 * \param cS  Chip select of NPSCx.
 */
extern void SPI_ChipSelect( Spi* spi, uint8_t cS)
{
	spi->SPI_MR |= SPI_MR_PCS_Msk ;
	spi->SPI_MR &= ~(SPI_MR_PCS ( cS )) ;
}

/**
 * \brief Configures SPI Mode Register.
 *
 * \param spi  Pointer to an SPI instance.
 * \param configuration  Value of the SPI mode register.
 */
extern void SPI_SetMode( Spi* spi, 
		uint32_t dwConfiguration )
{
	spi->SPI_MR = dwConfiguration ;
}

/**
 * \brief Configures SPI to release last used CS line.
 *
 * \param spi  Pointer to an SPI instance.
 */
extern void SPI_ReleaseCS( Spi* spi )
{
	spi->SPI_CR = SPI_CR_LASTXFER ;
}


/**
 * \brief Configures a chip select of a SPI peripheral. The chip select 
 * configuration is computed using several macros 
 * (see \ref spi_configuration_macros).
 *
 * \param spi   Pointer to an SPI instance.
 * \param npcs  Chip select to configure (0, 1, 2 or 3).
 * \param configuration  Desired chip select configuration.
 */
void SPI_ConfigureNPCS( Spi* spi, uint32_t dwNpcs, uint32_t dwConfiguration )
{
	spi->SPI_CSR[dwNpcs] = dwConfiguration ;
}

/**
 * \brief Configures a chip select active mode of a SPI peripheral.
 *
 * \param spi   Pointer to an SPI instance.
 * \param dwNpcs  Chip select to configure (0, 1, 2 or 3).
 * \param bReleaseOnLast CS controlled by last transfer.
 *                       SPI_ReleaseCS() is used to release CS. 
 */
void SPI_ConfigureCSMode( Spi* spi, uint32_t dwNpcs, uint32_t bReleaseOnLast )
{
	if (bReleaseOnLast) {
		spi->SPI_CSR[dwNpcs] |=  SPI_CSR_CSAAT;
	} else {
		spi->SPI_CSR[dwNpcs] &= ~SPI_CSR_CSAAT;
	}
}

/**
 * \brief Get the current status register of the given SPI peripheral.
 * \note This resets the internal value of the status register, so further
 * read may yield different values.
 * \param spi   Pointer to a Spi instance.
 * \return  SPI status register.
 */
extern uint32_t SPI_GetStatus( Spi* spi )
{
	return spi->SPI_SR ;
}

/**
 * \brief Reads and returns the last word of data received by a SPI peripheral. 
 * This method must be called after a successful SPI_Write call.
 *
 * \param spi  Pointer to an Spi instance.
 *
 * \return read data.
 */
extern uint32_t SPI_Read( Spi* spi )
{
	while ( (spi->SPI_SR & SPI_SR_RDRF) == 0 ) ;

	return spi->SPI_RDR & 0xFFFF ;
}

/**
 * \brief Sends data through a SPI peripheral. If the SPI is configured to use a 
 * fixed  peripheral select, the npcs value is meaningless. Otherwise,
 * it identifies the component which shall be addressed.
 *
 * \param spi   Pointer to an SPI instance.
 * \param npcs  Chip select of the component to address (0, 1, 2 or 3).
 * \param data  Word of data to send.
 */
extern void SPI_Write( Spi* spi, uint32_t dwNpcs, uint16_t wData )
{
	/* Send data */
	while ( (spi->SPI_SR & SPI_SR_TXEMPTY) == 0 ) ;
	spi->SPI_TDR = wData | SPI_PCS( dwNpcs ) ;
	while ( (spi->SPI_SR & SPI_SR_TDRE) == 0 ) ;
}

/**
 * \brief Sends last data through a SPI peripheral.
 * If the SPI is configured to use a fixed peripheral select, the npcs value is
 * meaningless. Otherwise, it identifies the component which shall be addressed.
 *
 * \param spi   Pointer to an SPI instance.
 * \param npcs  Chip select of the component to address (0, 1, 2 or 3).
 * \param data  Word of data to send.
 */
extern void SPI_WriteLast( Spi* spi, uint32_t dwNpcs, uint16_t wData )
{
	/* Send data */
	while ( (spi->SPI_SR & SPI_SR_TXEMPTY) == 0 ) ;
	spi->SPI_TDR = wData | SPI_PCS( dwNpcs ) | SPI_TDR_LASTXFER ;
	while ( (spi->SPI_SR & SPI_SR_TDRE) == 0 ) ;
}

/**
 * \brief Check if SPI transfer finish.
 *
 * \param spi  Pointer to an SPI instance.
 *
 * \return Returns 1 if there is no pending write operation on the SPI; 
 * otherwise returns 0.
 */
extern uint32_t SPI_IsFinished( Spi* spi )
{
	return ((spi->SPI_SR & SPI_SR_TXEMPTY) != 0) ;
}

