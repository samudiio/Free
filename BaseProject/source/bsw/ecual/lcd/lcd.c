
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
#define LCD_CS_PIN      0   //PD index
#define LCD_PD_PIN      1

#define SPI0_CS_3  3

/*------------------------------------------------------------------------------
 *         Global Variables
 *----------------------------------------------------------------------------*/

static const Pin LCD_PinsId[] = { PIN_LCD_CS, PIN_LCD_PD };

uint8_t pTxBuffer[] = "T";//"This is SPI LoopBack Test Buffer";
//uint8_t pTxBuffer ='S';

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

void LDC_Init(void)
{
    uint8_t i;
    uint32_t j;

    /* Configure CD and PD pins*/
    PIO_Configure( LCD_PinsId, PIO_LISTSIZE(LCD_PinsId) );

    /* Initialize SPI peripheral */
    SPI_Init();

   // SPI_Write(SPI0, SPI0_CS_3 , (uint16_t)pTxBuffer);

    for (i = 0; ;i++) {
        SPI_Write(SPI0, SPI0_CS_3 , (uint16_t)pTxBuffer[i]);
        if (pTxBuffer[i] =='\0')
            break;
    }

    /*if (SPI_IsFinished(SPI0)) {
        SPI_Disable(SPI0);
    } */

    LCD_PDlow();
    LCD_CSlow();
    for(j=0; j<65000; j++)
    {

    }
    LCD_CShigh();
    LCD_PDhigh();
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
    PIO_Clear( &LCD_PinsId[LCD_CS_PIN] );
}

/*
 * CS line high
 */
void LCD_CShigh(void)
{
    PIO_Set( &LCD_PinsId[LCD_CS_PIN] );
}

/*
 * PD line low
 */
void LCD_PDlow(void)
{
    PIO_Clear( &LCD_PinsId[LCD_PD_PIN] );
}

/*
 * PD line high
 */
void LCD_PDhigh(void)
{
    PIO_Set( &LCD_PinsId[LCD_PD_PIN] );
}
