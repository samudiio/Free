
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

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

void LCD_Init(void)
{
    /* Configure /CD and /PD pins*/
    PIO_Configure( LCD_PinsId, PIO_LISTSIZE(LCD_PinsId) );

    /* Initialize SPI peripheral */
    SPI_Init();

/*
    LCD_PDlow();
    LCD_CSlow();
    MCU_Delay_500ms();
    LCD_CShigh();
    LCD_PDhigh();

    LCD_SPIReadWrite(0x4C);
*/
}

/* Change clock frequency to 25mhz */
void LCD_ChangeClock(void)
{
    SPI_ChangeClock();
}

// ################################# SPI #######################################

uint8_t LCD_SPIReadWrite(uint8_t DataToWrite)
{
    return SPI_ReadWrite(DataToWrite);
}


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

void MCU_Delay_20ms(void)
{
    uint32_t dly;

    for(dly=0; dly< 0x40006; dly++)
    {

    }
}

void MCU_Delay_500ms(void)
{
    uint32_t dly;

    for(dly=0; dly< 0x62FFFF; dly++)
    {

    }
}
