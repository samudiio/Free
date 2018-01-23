
/**
 * \lcd.c
 *
 * This layer provides an interface to the MCU hardware.
 * It takes the SPI transfers from the EVE layer and translates them into the register level operations
 * needed to control the MCU’s SPI Master peripheral as well as GPIO operations for chip select and power down.
 *
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "board.h"
#include "lcd.h"
#include "spi.h"

//#include "FT8xx.h"

/*------------------------------------------------------------------------------
 *         Defines
 *----------------------------------------------------------------------------*/
#define LCD_PD  0   //PD index
#define SPI0_CS_3  3

/*------------------------------------------------------------------------------
 *         Global Variables
 *----------------------------------------------------------------------------*/

static const Pin LCD_PinsId[] = { PIN_LCD_PD };

uint8_t pTxBuffer[] = "This is SPI LoopBack Test Buffer";

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

void LDC_Init(void)
{
    uint8_t i;
    uint32_t j;

    /* Configure PD pin*/
    PIO_Configure( LCD_PinsId, PIO_LISTSIZE(LCD_PinsId) );

    /* Initialize SPI peripheral */
    SPI_Init();

    for (i = 0; ;i++) {
        SPI_Write(SPI0, SPI0_CS_3 , (uint16_t)pTxBuffer[i]);
        if (pTxBuffer[i] =='\0')
            break;
    }
    if (SPI_IsFinished(SPI0)) {
        SPI_Disable(SPI0);
    }


    for(j=0; j<65000; j++)
    {

    }
}

// ################################# SPI #######################################

uint8_t LCD_SPIReadWrite(uint8_t DataToWrite)
{
    return SPI_ReadWrite(DataToWrite);
}

//Wait(20);

// ########################### GPIO CONTROL ####################################

/*
 * PD line low
 */
void LCD_PDlow(void)
{
    PIO_Clear( &LCD_PinsId[LCD_PD] );
}

/*
 * PD line high
 */
void LCD_PDhigh(void)
{
    PIO_Set( &LCD_PinsId[LCD_PD] );
}







