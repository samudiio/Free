/* ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2014, Atmel Corporation
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

/** \file */

/** \addtogroup dmac_module Working with DMAC
 *
 * \section Usage
 * <ul>
 * <li> Enable or disable the a DMAC controller with DMAC_Enable() and or 
 * DMAC_Disable().</li>
 * <li> Enable or disable %Dma interrupt using DMAC_EnableIt()or 
 * DMAC_DisableIt().</li>
 * <li> Get %Dma interrupt status by DMAC_GetStatus() and 
 * DMAC_GetInterruptMask().</li>
 * <li> Enable or disable specified %Dma channel with DMAC_EnableChannel() or 
 * DMAC_DisableChannel().</li>
 * <li> Get %Dma channel status by DMAC_GetChannelStatus().</li>
 * <li> ControlA and ControlB register is set by DMAC_SetControlA() and 
 * DMAC_SetControlB().</li>
 * <li> Configure source and/or destination start address with 
 * DMAC_SetSourceAddr() and/or DMAC_SetDestinationAddr().</li>
 * <li> Set %Dma descriptor address using DMAC_SetDescriptorAddr().</li>
 * <li> Set source transfer buffer size with DMAC_SetBufferSize().</li>
 * <li> Configure source and/or destination Picture-In-Picuture mode with 
 * DMAC_SetSourcePip() and/or DMAC_SetDestPip().</li>
 * </ul>
 *
 * For more accurate information, please look at the DMAC section of the
 * Datasheet.
 *
 * \sa \ref dmad_module
 *
 * Related files :\n
 * \ref dmac.c\n
 * \ref dmac.h.\n
 *
 */

#ifndef DMAC_H
#define DMAC_H
/**@{*/

/*------------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

#include <assert.h>
#include <utility.h>
#include <stdint.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/

/** \addtogroup dmac_defines DMAC Definitions
 *      @{
 */
/** Number of DMA channels */
#define XDMAC_CONTROLLER_NUM            1
/** Number of DMA channels */
#define XDMAC_CHANNEL_NUM               24
/** Max DMA single transfer size */
#define XDMAC_MAX_BT_SIZE               0xFFFF
/**     @}*/

/** DMA channel hardware interface number */
#define XDAMC_CHANNEL_HWID_HSMCI       0
#define XDAMC_CHANNEL_HWID_SPI0_TX     1
#define XDAMC_CHANNEL_HWID_SPI0_RX     2
#define XDAMC_CHANNEL_HWID_SPI1_TX     3
#define XDAMC_CHANNEL_HWID_SPI1_RX     4
#define XDAMC_CHANNEL_HWID_QSPI_TX     5
#define XDAMC_CHANNEL_HWID_QSPI_RX     6
#define XDAMC_CHANNEL_HWID_USART0_TX   7
#define XDAMC_CHANNEL_HWID_USART0_RX   8
#define XDAMC_CHANNEL_HWID_USART1_TX   9
#define XDAMC_CHANNEL_HWID_USART1_RX   10
#define XDAMC_CHANNEL_HWID_USART2_TX   11
#define XDAMC_CHANNEL_HWID_USART2_RX   12
#define XDAMC_CHANNEL_HWID_PWM0        13
#define XDAMC_CHANNEL_HWID_TWIHS0_TX   14
#define XDAMC_CHANNEL_HWID_TWIHS0_RX   15
#define XDAMC_CHANNEL_HWID_TWIHS1_TX   16
#define XDAMC_CHANNEL_HWID_TWIHS1_RX   17
#define XDAMC_CHANNEL_HWID_TWIHS2_TX   18
#define XDAMC_CHANNEL_HWID_TWIHS2_RX   19
#define XDAMC_CHANNEL_HWID_UART0_TX    20
#define XDAMC_CHANNEL_HWID_UART0_RX    21
#define XDAMC_CHANNEL_HWID_UART1_TX    22
#define XDAMC_CHANNEL_HWID_UART1_RX    23
#define XDAMC_CHANNEL_HWID_UART2_TX    24
#define XDAMC_CHANNEL_HWID_UART2_RX    25
#define XDAMC_CHANNEL_HWID_UART3_TX    26
#define XDAMC_CHANNEL_HWID_UART3_RX    27
#define XDAMC_CHANNEL_HWID_UART4_TX    28
#define XDAMC_CHANNEL_HWID_UART4_RX    29
#define XDAMC_CHANNEL_HWID_DAC         30
#define XDAMC_CHANNEL_HWID_SSC_TX      32
#define XDAMC_CHANNEL_HWID_SSC_RX      33
#define XDAMC_CHANNEL_HWID_PIOA        34
#define XDAMC_CHANNEL_HWID_AFEC0       35
#define XDAMC_CHANNEL_HWID_AFEC1       36
#define XDAMC_CHANNEL_HWID_AES_TX      37
#define XDAMC_CHANNEL_HWID_AES_RX      38
#define XDAMC_CHANNEL_HWID_PWM1        39
#define XDAMC_CHANNEL_HWID_TC0         40
#define XDAMC_CHANNEL_HWID_TC1         41
#define XDAMC_CHANNEL_HWID_TC2         42
#define XDAMC_CHANNEL_HWID_TC3         43

/*----------------------------------------------------------------------------
 *         Macro
 *----------------------------------------------------------------------------*/
#define XDMA_GET_DATASIZE(size) ((size==0)? XDMAC_CC_DWIDTH_BYTE : \
								((size==1)? XDMAC_CC_DWIDTH_HALFWORD : \
								(XDMAC_CC_DWIDTH_WORD  )))
#define XDMA_GET_CC_SAM(s)      ((s==0)? XDMAC_CC_SAM_FIXED_AM : \
								((s==1)? XDMAC_CC_SAM_INCREMENTED_AM : \
								((s==2)? XDMAC_CC_SAM_UBS_AM : \
									XDMAC_CC_SAM_UBS_DS_AM )))
#define XDMA_GET_CC_DAM(d)      ((d==0)? XDMAC_CC_DAM_FIXED_AM : \
								((d==1)? XDMAC_CC_DAM_INCREMENTED_AM : \
								((d==2)? XDMAC_CC_DAM_UBS_AM : \
									XDMAC_CC_DAM_UBS_DS_AM )))
#define XDMA_GET_CC_MEMSET(m)   ((m==0)? XDMAC_CC_MEMSET_NORMAL_MODE : \
									XDMAC_CC_MEMSET_HW_MODE)

/*------------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/
/** \addtogroup dmac_functions
 *      @{
 */

#ifdef __cplusplus
 extern "C" {
#endif

extern uint32_t XDMAC_GetType( Xdmac *pXdmac);
extern uint32_t XDMAC_GetConfig( Xdmac *pXdmac);
extern uint32_t XDMAC_GetArbiter( Xdmac *pXdmac);
extern void XDMAC_EnableGIt (Xdmac *pXdmac, uint8_t dwInteruptMask );
extern void XDMAC_DisableGIt (Xdmac *pXdmac, uint8_t dwInteruptMask );
extern uint32_t XDMAC_GetGItMask( Xdmac *pXdmac );
extern uint32_t XDMAC_GetGIsr( Xdmac *pXdmac );
extern uint32_t XDMAC_GetMaskedGIsr( Xdmac *pXdmac );
extern void XDMAC_EnableChannel( Xdmac *pXdmac, uint8_t channel );
extern void XDMAC_EnableChannels( Xdmac *pXdmac, uint32_t bmChannels );
extern void XDMAC_DisableChannel( Xdmac *pXdmac, uint8_t channel );
extern void XDMAC_DisableChannels( Xdmac *pXdmac, uint32_t bmChannels );
extern uint32_t XDMAC_GetGlobalChStatus(Xdmac *pXdmac);
extern void XDMAC_SuspendReadChannel( Xdmac *pXdmac, uint8_t channel );
extern void XDMAC_SuspendWriteChannel( Xdmac *pXdmac, uint8_t channel );
extern void XDMAC_SuspendReadWriteChannel( Xdmac *pXdmac, uint8_t channel );
extern void XDMAC_ResumeReadWriteChannel( Xdmac *pXdmac, uint8_t channel );
extern void XDMAC_SoftwareTransferReq(Xdmac *pXdmac, uint8_t channel);
extern uint32_t XDMAC_GetSoftwareTransferStatus(Xdmac *pXdmac);
extern void XDMAC_SoftwareFlushReq(Xdmac *pXdmac, uint8_t channel);
extern void XDMAC_EnableChannelIt (Xdmac *pXdmac, uint8_t channel, 
			uint8_t dwInteruptMask );
extern void XDMAC_DisableChannelIt (Xdmac *pXdmac, uint8_t channel, 
			uint8_t dwInteruptMask );
extern uint32_t XDMAC_GetChannelItMask (Xdmac *pXdmac, uint8_t channel);
extern uint32_t XDMAC_GetChannelIsr (Xdmac *pXdmac, uint8_t channel);
extern uint32_t XDMAC_GetMaskChannelIsr (Xdmac *pXdmac, uint8_t channel);
extern void XDMAC_SetSourceAddr(Xdmac *pXdmac, uint8_t channel, uint32_t addr);
extern void XDMAC_SetDestinationAddr(Xdmac *pXdmac, uint8_t channel, 
			uint32_t addr);
extern void XDMAC_SetDescriptorAddr(Xdmac *pXdmac, uint8_t channel, 
			uint32_t addr, uint8_t ndaif);
extern void XDMAC_SetDescriptorControl(Xdmac *pXdmac, uint8_t channel,
			uint8_t config);
extern void XDMAC_SetMicroblockControl(Xdmac *pXdmac, uint8_t channel, 
			uint32_t ublen);
extern void XDMAC_SetBlockControl(Xdmac *pXdmac, uint8_t channel,
			uint16_t blen);
extern void XDMAC_SetChannelConfig(Xdmac *pXdmac, uint8_t channel, 
			uint32_t config);
extern uint32_t XDMAC_GetChannelConfig(Xdmac *pXdmac, uint8_t channel);
extern void XDMAC_SetDataStride_MemPattern(Xdmac *pXdmac, uint8_t channel,
			uint32_t dds_msp);
extern void XDMAC_SetSourceMicroBlockStride(Xdmac *pXdmac, uint8_t channel, 
			uint32_t subs);
extern void XDMAC_SetDestinationMicroBlockStride(Xdmac *pXdmac, uint8_t channel, 
			uint32_t dubs);
extern uint32_t XDMAC_GetChDestinationAddr(Xdmac *pXdmac, uint8_t channel);
#ifdef __cplusplus
}
#endif

/**     @}*/
/**@}*/
#endif //#ifndef DMAC_H

