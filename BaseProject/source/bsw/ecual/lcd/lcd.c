
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

}

// ################################# SPI #######################################

uint8_t LCD_SPIReadWrite(uint8_t DataToWrite)
{
    return SPI_ReadWrite(DataToWrite);
}

//Wait(20);

// ########################### GPIO CONTROL ####################################

/*
 * CS line low
 */
void LCD_CSlow(void)
{
    SPI_CSlow();
}

/*
 * CD line high
 */
void LCD_CShigh(void)
{
    SPI_CShigh();
}

/*
 * PD line low
 */
void LCD_PDlow(void)
{
    /* PD# line low */
    if ( LCD_PinsId[LCD_PD].type == PIO_OUTPUT_0 )
    {
        PIO_Clear( &LCD_PinsId[LCD_PD] );
    } else {
        PIO_Set( &LCD_PinsId[LCD_PD] );
    }
}

/*
 * PD line high
 */
void LCD_PDhigh(void)
{
    /* PD# line high */
    if ( LCD_PinsId[LCD_PD].type == PIO_OUTPUT_0 )
    {
        PIO_Set( &LCD_PinsId[LCD_PD] );
    } else {
        PIO_Clear( &LCD_PinsId[LCD_PD] );
    }
}







