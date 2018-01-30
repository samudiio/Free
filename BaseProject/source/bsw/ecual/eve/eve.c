/*
 * \eve.c
 *
 * This layer is called by the application layer functions and translates the display content-oriented calls from the main application
 * into a series of SPI transfers formatted for the protocol used by the FT813.
 * This allows the application layer above to focus on the screen content rather than the raw SPI values.
 *
 */


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
//#include "xc.h"       //?? not inlcuded

#include "eve.h"

/*------------------------------------------------------------------------------
 *         Defines
 *----------------------------------------------------------------------------*/

#define MEM_WRITE   0x80            // FT800 Host Memory Write
#define MEM_READ    0x00            // FT800 Host Memory Read

//##############################################################################
//##############################################################################
//                        EVE INTERFACE LAYER
//##############################################################################
//##############################################################################


/*----------------------------------------------------------------------------
 *        INDIVIDUAL ADDRESSING AND DATA FUNCTIONS
 *----------------------------------------------------------------------------*/

// This section has separate calls for addressing and data phases of the transaction which allows for burst writes and reads.

// ------------------ Send FT81x register address for writing ------------------
void EVE_AddrForWr(uint32_t ftAddress)
{
    uint8_t dummyRead = 0;
    // Send three bytes of a register address which has to be subsequently written. Ignore return values as this is an SPI write only.
    dummyRead = LCD_SPIReadWrite( (uint8_t) ((ftAddress >> 16) | MEM_WRITE) );     // Send high byte of address with 'write' bits set

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftAddress >> 8) );                    // Send middle byte of address

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftAddress) );                         // Send low byte of address
}

// ------------------ Send FT81x register address for reading ------------------
void EVE_AddrForRd(uint32_t ftAddress)
{
    uint8_t dummyRead = 0;
    // Send three bytes of a register address which has to be subsequently read. Ignore return values as this is an SPI write only.
    dummyRead = LCD_SPIReadWrite( (uint8_t) ((ftAddress >> 16) | MEM_READ) );      // Send high byte of address with 'read' bits set

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftAddress >> 8) );                    // Send middle byte of address

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftAddress) );                         // Send low byte of address

    dummyRead = LCD_SPIReadWrite( 0x00 );                                       // Send dummy byte before doing the read
}

// ------------------------ Send a 32-bit data value --------------------------
void EVE_Write32(uint32_t ftData32)
{
    uint8_t dummyRead = 0;
    // Send four bytes of data after previously sending address. Ignore return values as this is an SPI write only.
    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftData32) );                          // Send low byte of data

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftData32 >> 8) );                     //

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftData32 >> 16) );                    //

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftData32 >> 24) );                    // Send high byte of data
}

// ------------------------ Send a 16-bit data value --------------------------
void EVE_Write16(uint16_t ftData16)
{

    uint8_t dummyRead = 0;
    // Send two bytes of data after previously sending address. Ignore return values as this is an SPI write only.
    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftData16) );                          // Send low byte of data

    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftData16 >> 8) );                     // Send high byte of data
}

// ------------------------ Send an 8-bit data value ---------------------------
void EVE_Write8(uint8_t ftData8)
{
    uint8_t dummyRead = 0;
    // Send one byte of data after previously sending address. Ignore return values as this is an SPI write only.
    dummyRead = LCD_SPIReadWrite( (uint8_t) (ftData8) );                          // Send byte of data
}

// ------------------------ Read a 32-bit data value --------------------------
uint32_t EVE_Read32(void)
{
    // Read 4 bytes from a register has been previously addressed. Send dummy 00 bytes as only the incoming value is important.
    uint32_t ftData32 = 0x00000000;
    uint32_t tempData32 = 0x00000000;

    tempData32 = LCD_SPIReadWrite( 0x00 );                                      // Read low byte of data first
    ftData32 = ftData32 | tempData32;

    tempData32 = LCD_SPIReadWrite( 0x00 );
    tempData32 = ((tempData32 << 8) & 0x0000FF00);
    ftData32 = ftData32 | tempData32;

    tempData32 = LCD_SPIReadWrite( 0x00 );
    tempData32 = ((tempData32 << 16) & 0x00FF0000);
    ftData32 = ftData32 | tempData32;

    tempData32 = LCD_SPIReadWrite( 0x00 );                                      // Read high byte of data last
    tempData32 = ((tempData32 << 24) & 0xFF000000);
    ftData32 = ftData32 | tempData32;

    return ftData32;                                                            // Return combined 32-bit value
}

// ------------------------ Read a 16-bit data value ---------------------------
uint16_t EVE_Read16(void)
{
    // Read 2 bytes from a register has been previously addressed. Send dummy 00 bytes as only the incoming value is important.
    uint16_t ftData16 = 0x0000;
    uint16_t tempData16 = 0x0000;

    tempData16 = LCD_SPIReadWrite( 0x00 );                                      // Read low byte of data first
    ftData16 = ftData16 | tempData16;

    tempData16 = LCD_SPIReadWrite( 0x00 );                                      // Read high byte of data last
    tempData16 = ((tempData16 << 8) & 0xFF00);
    ftData16 = ftData16 | tempData16;

    return ftData16;                                                            // Return combined 16-bit value
}

// ------------------------ Read an 8-bit data value ---------------------------
uint8_t EVE_Read8(void)
{
    // Read 1 byte from a register has been previously addressed. Send dummy 00 byte as only the incoming value is important.
    uint8_t ftData8 = 0x00;

    ftData8 = LCD_SPIReadWrite( 0x00 );                                         // Read byte of data

    return ftData8;                                                             // Return 8-bit value read
}

/*----------------------------------------------------------------------------
 *        COMBINED ADDRESSING AND DATA FUNCTIONS
 *----------------------------------------------------------------------------*/

// This section has combined calls which carry out a full write or read cycle including chip select, address, and data transfer.
// This would often be used for register writes and reads.

// -------------- Write a 32-bit value to specified address --------------------
void EVE_MemWrite32(uint32_t ftAddress, uint32_t ftData32)
{
    LCD_CSlow();                                                                // CS low begins the SPI transfer

    EVE_AddrForWr(ftAddress);                                                   // Send address to be written

    EVE_Write32(ftData32);                                                      // Send the data value

    LCD_CShigh();                                                               // CS high terminates the SPI transfer
}

// -------------- Write a 16-bit value to specified address --------------------
void EVE_MemWrite16(uint32_t ftAddress, uint16_t ftData16)
{
    LCD_CSlow();                                                                // CS low begins the SPI transfer

    EVE_AddrForWr(ftAddress);                                                   // Send address to be written

    EVE_Write16(ftData16);                                                      // Send the data value

    LCD_CShigh();                                                               // CS high terminates the SPI transfer
}

// -------------- Write an 8-bit value to specified address --------------------
void EVE_MemWrite8(uint32_t ftAddress, uint8_t ftData8)
{
    LCD_CSlow();                                                                // CS low begins the SPI transfer

    EVE_AddrForWr(ftAddress);                                                   // Send address to be written

    EVE_Write8(ftData8);                                                        // Send the data value

    LCD_CShigh();                                                               // CS high terminates the SPI transfer
}



// -------------- Read a 32-bit value from specified address --------------------
uint32_t EVE_MemRead32(uint32_t ftAddress)
{
    uint32_t ftData32 = 0x00000000;

    LCD_CSlow();                                                                // CS low begins the SPI transfer

    EVE_AddrForRd(ftAddress);                                                   // Send address to be read

    ftData32 = EVE_Read32();                                                    // Read the data value

    LCD_CShigh();                                                               // CS high terminates the SPI transfer

    return ftData32;                                                            // Return 32-bit value read
}

// -------------- Read a 16-bit value from specified address --------------------
uint16_t EVE_MemRead16(uint32_t ftAddress)
{
    uint16_t ftData16 = 0x0000;

    LCD_CSlow();                                                                // CS low begins the SPI transfer

    EVE_AddrForRd(ftAddress);                                                   // Send address to be read

    ftData16 = EVE_Read16();                                                    // Read the data value

    LCD_CShigh();                                                               // CS high terminates the SPI transfer

    return ftData16;                                                            // Return 16-bit value read
}

// -------------- Read an 8-bit value from specified address --------------------
uint8_t EVE_MemRead8(uint32_t ftAddress)
{
    uint8_t ftData8 = 0x00;

    LCD_CSlow();                                                                // CS low begins the SPI transfer

    EVE_AddrForRd(ftAddress);                                                   // Send address to be read

    ftData8 = EVE_Read8();                                                      // Read the data value

    LCD_CShigh();                                                               // CS high terminates the SPI transfer

    return ftData8;                                                             // Return 8-bit value read
}



/*----------------------------------------------------------------------------
 *        HOST COMMANDS
 *----------------------------------------------------------------------------*/

// -------------------------- Write a host command -----------------------------
void EVE_CmdWrite(uint8_t EVECmd, uint8_t Param)
{
    uint8_t dummyRead = 0;

    LCD_CSlow();                                                                // CS low begins the SPI transfer

    dummyRead = LCD_SPIReadWrite( (uint8_t) (EVECmd) );                            // Send command

    dummyRead = LCD_SPIReadWrite( (uint8_t) (Param) );                             // followed by parameter

    dummyRead = LCD_SPIReadWrite( 0x00 );                                       // and a dummy 00 byte

    LCD_CShigh();                                                               // CS high terminates the SPI transfer
}

/*----------------------------------------------------------------------------
 *        SUPPORTING FUNCTIONS
 *----------------------------------------------------------------------------*/

// --------- Increment co-processor address offset counter --------------------
uint16_t EVE_IncCMDOffset(uint16_t currentOffset, uint8_t commandSize)
{
    uint16_t newOffset;                                                     // Used to hold new offset

    newOffset = currentOffset + commandSize;                                    // Calculate new offset

    if(newOffset > 4095)                                                        // If new offset past top of buffer...
    {
        newOffset = (newOffset - 4096);                                         // ... roll over
    }

    return newOffset;                                                           // Return new offset
}

// ------ Wait for co-processor read and write pointers to be equal ------------

uint16_t EVE_WaitCmdFifoEmpty(void)
{
    uint16_t ReadPointer, WritePointer;

    do
    {
        ReadPointer = (uint16_t)EVE_MemRead32(REG_CMD_READ);                              // Read the graphics processor read pointer
        WritePointer = (uint16_t)EVE_MemRead32(REG_CMD_WRITE);                            // Read the graphics processor write pointer
    }while (WritePointer != ReadPointer);                                       // Wait until the two registers match

    return WritePointer;                                                        // New starting point will be first location after the last command
}



