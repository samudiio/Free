/*
 * \eve.h
 *

 */

#ifndef EVE_H_
#define EVE_H_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"
#include "FT8xx.h"

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *        INDIVIDUAL ADDRESSING AND DATA FUNCTIONS
 *----------------------------------------------------------------------------*/
/*
 * Send FT81x register address for writing
 */
void EVE_AddrForWr(uint32_t ftAddress);

/*
 * Send FT81x register address for reading
 */
void EVE_AddrForRd(uint32_t ftAddress);

/*
 * Send a 32-bit data value
 */
void EVE_Write32(uint32_t ftData32);

/*
 * Send a 16-bit data value
 */
void EVE_Write16(uint16_t ftData16);

/*
 * Send an 8-bit data value
 */
void EVE_Write8(uint8_t ftData8);

/*
 * Read a 32-bit data value
 */
uint32_t EVE_Read32(void);

/*
 * Read a 16-bit data value
 */
uint16_t EVE_Read16(void);

/*
 * Read an 8-bit data value
 */
uint8_t EVE_Read8(void);


/*----------------------------------------------------------------------------
 *        COMBINED ADDRESSING AND DATA FUNCTIONS
 *----------------------------------------------------------------------------*/
/*
 * Write a 32-bit value to specified address
 */
void EVE_MemWrite32(uint32_t ftAddress, uint32_t ftData32);

/*
 * Write a 16-bit value to specified address
 */
void EVE_MemWrite16(uint32_t ftAddress, uint16_t ftData16);

/*
 * Write an 8-bit value to specified address
 */
void EVE_MemWrite8(uint32_t ftAddress, uint8_t ftData8);

/*
 * Read a 32-bit value from specified address
 */
uint32_t EVE_MemRead32(uint32_t ftAddress);

/*
 * Read a 16-bit value from specified address
 */
uint16_t EVE_MemRead16(uint32_t ftAddress);

/*
 * Read an 8-bit value from specified address
 */
uint8_t EVE_MemRead8(uint32_t ftAddress);


/*----------------------------------------------------------------------------
 *        HOST COMMANDS
 *----------------------------------------------------------------------------*/
/*
 * Write a host command
 */
void EVE_CmdWrite(uint8_t EVECmd, uint8_t Param);


/*----------------------------------------------------------------------------
 *        SUPPORTING FUNCTIONS
 *----------------------------------------------------------------------------*/
/*
 * Increment co-processor address offset counter
 */
uint16_t EVE_IncCMDOffset(uint16_t currentOffset, uint8_t commandSize);

/*
 * Wait for co-processor read and write pointers to be equal
 */
uint16_t EVE_WaitCmdFifoEmpty(void);

#endif /* EVE_H_ */
