/*
 * lcd_app.c
 *
 * This layer implements the main application. It contains the code for initialising the display and
 * then building up each application screen by creating co-processor lists.
 * It will also include header files where required for image and font data.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "chip.h"
#include "eve.h"
#include "lcd.h"
#include "lcd_app.h"


/*----------------------------------------------------------------------------
 *        Global variables
 *----------------------------------------------------------------------------*/
// LCD display parameters
uint16_t lcdWidth;              // Active width of LCD display
uint16_t lcdHeight;             // Active height of LCD display
uint16_t lcdHcycle;             // Total number of clocks per line
uint16_t lcdHoffset;            // Start of active line
uint16_t lcdHsync0;             // Start of horizontal sync pulse
uint16_t lcdHsync1;             // End of horizontal sync pulse
uint16_t lcdVcycle;             // Total number of lines per screen
uint16_t lcdVoffset;            // Start of active screen
uint16_t lcdVsync0;             // Start of vertical sync pulse
uint16_t lcdVsync1;             // End of vertical sync pulse
uint8_t lcdPclk;              // Pixel Clock
uint8_t lcdSwizzle;           // Define RGB output pins
uint8_t lcdPclkpol;           // Define active edge of PCLK

uint32_t ramDisplayList = 3145728UL;//RAM_DL;      // Set beginning of display list memory
uint32_t color;                // Variable for changing colors

uint16_t cmdOffset = 0x0000;        // Used to navigate command rung buffer
uint16_t point_size = 0x0100;       // Define a default dot size

uint8_t FT81x_GPIO;           // Used for FT800 GPIO register


#define NOCOMP 0

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

//########################### INITIALISE EVE ###################################

void APP_Init(void)
{
    // ----------------------- Cycle PD pin to reset device --------------------

    LCD_PDlow();                                                                // PD low to reset device

    MCU_Delay_20ms();

    LCD_PDhigh();                                                               // PD high again

    MCU_Delay_20ms();

    // ---------------------- Delay to allow start-up --------------------

    EVE_CmdWrite(FT81x_ACTIVE, 0x00);                                           // Sends 00 00 00 to wake FT8xx

    MCU_Delay_500ms();                                                          // 500ms delay (EVE requires at least 300ms here))

    // --------------- Check that FT8xx ready and SPI comms OK -----------------

    while (EVE_MemRead8(REG_ID) != 0x7C)                                        // Read REG_ID register (0x302000) until reads 0x7C
    {
    }

    while (EVE_MemRead8(REG_CPURESET) != 0x00)                                  // Ensure CPUreset register reads 0 and so FT8xx is ready
    {
    }

    // ------------------------- Display settings ------------------------------

    // WQVGA display parameters
    lcdWidth   = 480;                                                           // Active width of LCD display
    lcdHeight  = 272;                                                           // Active height of LCD display
    lcdHcycle  = 928;                                                           // Total number of clocks per line
    lcdHoffset = 88;                                                            // Start of active line
    lcdHsync0  = 0;                                                             // Start of horizontal sync pulse
    lcdHsync1  = 48;                                                            // End of horizontal sync pulse
    lcdVcycle  = 525;                                                           // Total number of lines per screen
    lcdVoffset = 32;                                                            // Start of active screen
    lcdVsync0  = 0;                                                             // Start of vertical sync pulse
    lcdVsync1  = 3;                                                             // End of vertical sync pulse
    lcdPclk    = 2;                                                             // Pixel Clock
    lcdSwizzle = 0;                                                             // Define RGB output pins
    lcdPclkpol = 1;                                                             // Define active edge of PCLK

    EVE_MemWrite16(REG_HSIZE,   lcdWidth);
    EVE_MemWrite16(REG_HCYCLE,  lcdHcycle);
    EVE_MemWrite16(REG_HOFFSET, lcdHoffset);
    EVE_MemWrite16(REG_HSYNC0,  lcdHsync0);
    EVE_MemWrite16(REG_HSYNC1,  lcdHsync1);
    EVE_MemWrite16(REG_VSIZE,   lcdHeight);
    EVE_MemWrite16(REG_VCYCLE,  lcdVcycle);
    EVE_MemWrite16(REG_VOFFSET, lcdVoffset);
    EVE_MemWrite16(REG_VSYNC0,  lcdVsync0);
    EVE_MemWrite16(REG_VSYNC1,  lcdVsync1);
    EVE_MemWrite8(REG_SWIZZLE,  lcdSwizzle);
    EVE_MemWrite8(REG_PCLK_POL, lcdPclkpol);

    FT81x_GPIO = EVE_MemRead8(REG_GPIO);                                        // Read the FT800 GPIO register for a read/modify/write operation
    FT81x_GPIO = FT81x_GPIO | 0x80;                                             // set bit 7 of FT800 GPIO register (DISP) - others are inputs
    EVE_MemWrite8(REG_GPIO, FT81x_GPIO);                                        // Enable the DISP signal to the LCD panel

    // Can move these 2 lines to after the first display list to make the start-up appear cleaner to the user
    EVE_MemWrite8(REG_PCLK, lcdPclk);                                           // Now start clocking data to the LCD panel
    EVE_MemWrite8(REG_PWM_DUTY, 127);

    // ---------------------- Touch and Audio settings -------------------------

//    EVE_MemWrite16(REG_TOUCH_RZTHRESH, 1200);                                   // Eliminate any false touches
//
//    EVE_MemWrite8(REG_VOL_PB, ZERO);                                            // turn recorded audio volume down
//    EVE_MemWrite8(REG_VOL_SOUND, ZERO);                                         // turn synthesizer volume down
//    EVE_MemWrite16(REG_SOUND, 0x6000);                                          // set synthesizer to mute

    // -------- Initial display list to begin with blank screen ----------------

    ramDisplayList = RAM_DL;                                                    // start of Display List
    EVE_MemWrite32(ramDisplayList, 0x02000000);                                 // Clear Color RGB sets the colour to clear screen to

    ramDisplayList += 4;                                                        // point to next location
    EVE_MemWrite32(ramDisplayList, (0x26000000 | 0x00000007));                  // Clear 00100110 -------- -------- -----CST  (C/S/T define which parameters to clear)

    ramDisplayList += 4;                                                        // point to next location
    EVE_MemWrite32(ramDisplayList, 0x00000000);                                 // DISPLAY command 00000000 00000000 00000000 00000000 (end of display list)

    EVE_MemWrite32(REG_DLSWAP, DLSWAP_FRAME);                                   // Swap display list to make the edited one active
}


#if NOCOMP
// ######################## DEMO - FLASHING DOT ##############################

void APP_FlashingDot(void)
{

    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty and record current position in FIFO

    while(1)
    {
        if(color == 0x00)                                                       // Toggle color variable
        {
            color = 0xFF;
        }
        else
        {
            color = 0x00;
        }

        LCD_CSlow();                                                            // CS low begins SPI transaction
        EVE_AddrForWr(RAM_CMD + cmdOffset);                                     // Send address to which first value will be written

        EVE_Write32(CMD_DLSTART);                                               // Co-pro starts new DL at RAM_DL + 0
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                             // Keep count of bytes sent so that write pointer can be updated at end
                                                                                // Keeping CS low and FT8xx will auto increment address for 'burst write'

        EVE_Write32(CLEAR_COLOR_RGB(0,0,0));                                    // Set the default clear color to black
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(CLEAR(1,1,1));                                              // Clear the screen Attributes color, stencil and tag buffers
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32 (COLOR_RGB(color,0,0));                                     // Specify RGB colour of subsequent items drawn
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(BEGIN(FTPOINTS));                                           // Begin drawing points
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(POINT_SIZE(point_size));                                    // Select the size of the dot to draw
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(VERTEX2F(100*16,100*16));                                   // Set the point center location
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(END());                                                     // End drawing of points
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(DISPLAY());                                                 // Instruct the graphics processor to show the list
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(CMD_SWAP);                                                  // Make this list active
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        LCD_CShigh();                                                           // Chip Select high concludes burst

        EVE_MemWrite32(REG_CMD_WRITE, (cmdOffset));                             // Update the ring buffer pointer
                                                                                // Co-processor will now execute all of the above commands and create a display list
        cmdOffset = EVE_WaitCmdFifoEmpty();                                     // Await completion of processing and record starting address for next screen update

        MCU_Delay_500ms();                                                      // Delay to slow down the flashing of the dot to allow user to see it

    }
}

#endif


