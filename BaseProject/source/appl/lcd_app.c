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
#include <string.h>

//#include "chip.h"
#include "board.h"
#include "eve.h"
#include "lcd.h"
#include "lcd_app.h"
#include "uart.h"
//#include "EVE_RGB565.h"
#include "DIGITfont.h"


/*------------------------------------------------------------------------------
 *         Defines
 *----------------------------------------------------------------------------*/

#define NOCOMP      0

/* LCD display parameters 480x272
 * TFT Timing Characteristics
 */
#define LCD_HSIZE       480
#define LCD_HCYCLE      548     // Total number of clocks per line
#define LCD_HOFFSET     43      // Start of active line
#define LCD_HSYNC0      0       // Start of horizontal sync pulse
#define LCD_HSYNC1      41      // End of horizontal sync pulse
#define LCD_VSIZE       272
#define LCD_VCYCLE      292     // Total number of lines per screen
#define LCD_VOFFSET     12      // Start of active screen
#define LCD_VSYNC0      12      // Start of vertical sync pulse
#define LCD_VSYNC1      0       // End of vertical sync pulse
#define LCD_PCLK        5       // Pixel Clock
#define LCD_SWIZZLE     0       // Define RGB output pins
#define LCD_PCLKPOL     1       // Define active edge of PCLK
#define LCD_CSPREAD     1
#define LCD_DITHER      1

#define TOTAL_SIZE 261120       //Size of image arrays

/*----------------------------------------------------------------------------
 *        Global variables
 *----------------------------------------------------------------------------*/

uint32_t ramDisplayList = RAM_DL;      // Set beginning of display list memory
uint32_t color;                // Variable for changing colors
uint32_t TrackerVal = 0;// 32 bit

uint16_t cmdOffset = 0x0000;        // Used to navigate command rung buffer
uint16_t point_size = 0x0100;       // Define a default dot size
uint16_t SlideVal = 0;

uint8_t TagVal = 0;
uint8_t FT81x_GPIO;           // Used for FT800 GPIO register

/*('file properties: ', 'resolution ', 480, 'x', 272, 'format ', 'RGB565', 'stride ', 960, ' total size ', 261120)*/
/* Read buffer */
uint8_t data_buffer[TOTAL_SIZE];

const char bosch_logo_file_name[] = "0:Bosch_logo.raw";
const char cannondale_file_name[] = "0:cannondale.raw";
const char computing_file_name[] =  "0:computing.raw";
const char digital_file_name[] =    "0:digital.raw";
const char foco_file_name[] =       "0:foco.raw";
const char lightbulb_file_name[] =  "0:lightbulb.raw";
const char marketing_file_name[] =  "0:marketing.raw";
const char salomon_file_name[] =    "0:salomon.raw";
const char specialized_file_name[]= "0:specialized.raw";
const char trail_file_name[] =      "0:trail.raw";
const char utct_file_name[] =       "0:utct.raw";

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
    EVE_MemWrite16(REG_HSIZE,   LCD_HSIZE);      // Active width of LCD display
    EVE_MemWrite16(REG_HCYCLE,  LCD_HCYCLE);    // Total number of clocks per line
    EVE_MemWrite16(REG_HOFFSET, LCD_HOFFSET);   // Start of active line
    EVE_MemWrite16(REG_HSYNC0,  LCD_HSYNC0);    // Start of horizontal sync pulse
    EVE_MemWrite16(REG_HSYNC1,  LCD_HSYNC1);    // End of horizontal sync pulse
    EVE_MemWrite16(REG_VSIZE,   LCD_VSIZE);     // Active height of LCD display
    EVE_MemWrite16(REG_VCYCLE,  LCD_VCYCLE);    // Total number of lines per screen
    EVE_MemWrite16(REG_VOFFSET, LCD_VOFFSET);   // Start of active screen
    EVE_MemWrite16(REG_VSYNC0,  LCD_VSYNC0);    // Start of vertical sync pulse
    EVE_MemWrite16(REG_VSYNC1,  LCD_VSYNC1);    // End of vertical sync pulse
    EVE_MemWrite8(REG_SWIZZLE,  LCD_SWIZZLE);   // Define RGB output pins
    EVE_MemWrite8(REG_PCLK_POL, LCD_PCLKPOL);   // Define active edge of PCLK
    EVE_MemWrite8(REG_CSPREAD,  LCD_CSPREAD);
    EVE_MemWrite8(REG_DITHER,   LCD_DITHER);

    FT81x_GPIO = EVE_MemRead8(REG_GPIO);                                // Read the FT800 GPIO register for a read/modify/write operation
    FT81x_GPIO = FT81x_GPIO | 0x80;                                     // set bit 7 of FT800 GPIO register (DISP) - others are inputs
    EVE_MemWrite8(REG_GPIO, FT81x_GPIO);                                // Enable the DISP signal to the LCD panel

    // Can move these 2 lines to after the first display list to make the start-up appear cleaner to the user
    EVE_MemWrite8(REG_PCLK, LCD_PCLK);                                  // Now start clocking data to the LCD panel
    EVE_MemWrite8(REG_PWM_DUTY, 127);

    // ---------------------- Touch and Audio settings -------------------------

    EVE_MemWrite16(REG_TOUCH_RZTHRESH, 1200);                           // Eliminate any false touches

    EVE_MemWrite8(REG_VOL_PB, ZERO);                                    // turn recorded audio volume down
    EVE_MemWrite8(REG_VOL_SOUND, ZERO);                                 // turn synthesizer volume down
    EVE_MemWrite16(REG_SOUND, 0x6000);                                  // set synthesizer to mute

    // -------- Initial display list to begin with blank screen --------

    ramDisplayList = RAM_DL;                                            // start of Display List
    EVE_MemWrite32(ramDisplayList, CLEAR_COLOR_RGB(0,0,0));             // Clear Color RGB sets the colour to clear screen to black

    ramDisplayList += 4;                                                // point to next location
    EVE_MemWrite32(ramDisplayList, CLEAR(1,1,1));                       // Clear 00100110 -------- -------- -----CST  (C/S/T define which parameters to clear)

    ramDisplayList += 4;                                                // point to next location
    EVE_MemWrite32(ramDisplayList, DISPLAY());                          // DISPLAY command 00000000 00000000 00000000 00000000 (end of display list)

    EVE_MemWrite32(REG_DLSWAP, DLSWAP_FRAME);                           // Swap display list to make the edited one active

    LCD_ChangeClock();
}



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


// ######################## DEMO - VERTEX TRANSLATE ##########################

void APP_VertexTranslate(void)
  {

    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty and record current position in FIFO

    LCD_CSlow();                                                            // CS low begins SPI transaction
    EVE_AddrForWr(RAM_CMD + cmdOffset);                                     // Send address to which first value will be written

    EVE_Write32(CMD_DLSTART);                                               // Co-pro starts new DL at RAM_DL + 0
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                             // Keep count of bytes sent so that write pointer can be updated at end
                                                                            // Keeping CS low and FT8xx will auto increment address for 'burst write'

    EVE_Write32(CLEAR_COLOR_RGB(0,0,0));                                    // Set the default clear color to black
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(CLEAR(1,1,1));                                              // Clear the screen Attributes color, stencil and tag buffers
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(BEGIN(FTPOINTS));                                           // Begin drawing points
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(POINT_SIZE(point_size));                                    // Select the size of the dot to draw
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);


    EVE_Write32 (COLOR_RGB(0xFF,0,0));                                      // Colour = Red
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX2F(100*16,100*16));                                   // Draw a point at (100,100)
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);



    EVE_Write32 (COLOR_RGB(0,0xFF,0));                                      // Colour = Green
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX2F(200*16,100*16));                                   // Draw a point at (200,100)
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);



    EVE_Write32 (COLOR_RGB(0,0,0xFF));                                      // Colour = Blue
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX_TRANSLATE_X(150*16));                                // Add an offset of 150 to X direction of subsequent Vertex commands
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX2F(200*16,100*16));                                   // Draw a point at ((200+150),100) == (350,100))
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);



    EVE_Write32 (COLOR_RGB(0xFF,0,0xFF));                                   // Colour = Purple
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX2F(200*16,200*16));                                   // Draw a point at ((200+250),200) == (350,200))
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);



    EVE_Write32 (COLOR_RGB(0xFF,0xFF,0xFF));                                // Colour = White
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX_TRANSLATE_X(0*16));                                  // Return offset to 0 for subsequent Vertex commands
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX2F(300*16,200*16));                                   // Draw a point at (300,200)
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

    //APP_SnapShot2();

    /*while(1)
    {

    }*/
}


// ############################ DEMO - Text ##################################

void APP_Text(void)
{
    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty and record current position in FIFO

    LCD_CSlow();                                                                // CS low begins SPI transaction
    EVE_AddrForWr(RAM_CMD + cmdOffset);                                         // Send address to which first value will be written


    EVE_Write32(CMD_DLSTART);                                                   // Co-pro starts new DL at RAM_DL + 0
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Keep count of bytes sent so that write pointer can be updated at end
                                                                                // Keeping CS low and FT8xx will auto increment address for 'burst write'

    EVE_Write32(CLEAR_COLOR_RGB(0,0,0));                                        // Set the default clear color to black
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(CLEAR(1,1,1));                                                  // Clear the screen Attributes color, stencil and tag buffers
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32 (COLOR_RGB(255,255,255));                                       // Set colour to white
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32 (CMD_TEXT);                                                     // command text 0xFFFFFF0C
    EVE_Write16 (100);                                                          // x
    EVE_Write16 (100);                                                          // y
    EVE_Write16 (25);                                                           // font
    EVE_Write16 (0);                                                            // options

    EVE_Write8 (0x48);                                                          // string H
    EVE_Write8 (0x45);                                                          // string E
    EVE_Write8 (0x4C);                                                          // string L
    EVE_Write8 (0x4C);                                                          // string L

    EVE_Write8 (0x4F);                                                          // string O
    EVE_Write8 (0x20);                                                          // string space
    EVE_Write8 (0x41);                                                          // A
    EVE_Write8 (0x54);                                                          // T

    EVE_Write8 (0x48);                                                          // H
    EVE_Write8 (0x5A);                                                          // Z
    EVE_Write8 (0x49);                                                          // I
    EVE_Write8 (0x52);                                                          // R

    EVE_Write8 (0x49);                                                          // I
    EVE_Write8 (0);                                                             // null
    EVE_Write8 (0);                                                             // padding
    EVE_Write8 (0);                                                             // padding since overall command not multiple of 4

    cmdOffset = EVE_IncCMDOffset(cmdOffset, 28);                                // Update the command pointer, must be multiple of 4

    EVE_Write32(DISPLAY());                                                     // Instruct the graphics processor to show the list
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(CMD_SWAP);                                                      // Make this list active
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    LCD_CShigh();                                                               // Chip Select high concludes burst

    EVE_MemWrite32(REG_CMD_WRITE, (cmdOffset));                                 // Update the ring buffer pointer
                                                                                // Co-processor will now execute all of the above commands and create a display list

    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Await completion of processing and record starting address for next screen update

    //APP_SnapShot2();

   /* while(1)
    {

    }*/
}

void APP_WaitforSDCard(void)
{
    Ctrl_status status;

    printf("Please plug an SD card in slot.\n\r");

    /* Wait card present and ready */
    do {
        status = sd_mmc_test_unit_ready(0);
        if (CTRL_FAIL == status) {
            printf("Card install FAIL\n\r");
            printf("Please unplug and re-plug the card.\n\r");
            while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
            }
        }
    } while (CTRL_GOOD != status);
}


FRESULT APP_ReadImagefromSD(uint8_t file_number)
{
   FATFS fs;
   FRESULT res;
   FIL file_object;

   uint8_t * const file_names[11] =
   {
       (uint8_t*)cannondale_file_name,
       (uint8_t*)computing_file_name,
       (uint8_t*)digital_file_name,
       (uint8_t*)foco_file_name,
       (uint8_t*)lightbulb_file_name,
       (uint8_t*)marketing_file_name,
       (uint8_t*)salomon_file_name,
       (uint8_t*)specialized_file_name,
       (uint8_t*)trail_file_name,
       (uint8_t*)utct_file_name,
       (uint8_t*)bosch_logo_file_name
   };

   memset(&fs, 0, sizeof(FATFS));
   res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
   if (FR_INVALID_DRIVE == res) {
       printf("Mount [FAIL] res %d\n\r", res);
       return res;
   }

   /***** Start reading files procedure ****/
   /* Open the file */
   res = f_open(&file_object, (char const *)file_names[file_number], FA_OPEN_EXISTING | FA_READ);
   if (res != FR_OK) {
       printf("Open file [FAIL] res %d\n\r", res);
       return res;
   }

   /* Read file */
   memset(data_buffer, 0, TOTAL_SIZE);
   f_gets(data_buffer, TOTAL_SIZE, &file_object);


   /* Close the file*/
   f_close(&file_object);

   return FR_OK;
}

// ############################ DEMO - Bitmap ##################################

void APP_ConvertedBitmap_FirstTime(void)
{
    uint32_t DataPointer = 0;
    uint32_t DataSize = 261120;
    uint32_t BitmapDataSize = 0;

    APP_ReadImagefromSD(0);
    // ------------ Load image data -------------

    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty

    DataPointer = 0;

    LCD_CSlow();                                                                // CS low begins SPI transaction
    EVE_AddrForWr(RAM_G);                                                       // Send address to which first value will be written

    while(DataPointer < DataSize)
    {
        EVE_Write8(data_buffer[DataPointer]);                                   // Send data byte-by-byte from array
        DataPointer++;
    }

    BitmapDataSize = DataSize - DataPointer;                                    // Add 3, 2 or 1 bytes padding to make it  a multiple of 4 bytes
    BitmapDataSize = BitmapDataSize & 0x03;                                     // Mask off the bottom 2 bits

    if (BitmapDataSize == 0x03)
    {
        EVE_Write8(0x00);
    }
    else if (BitmapDataSize == 0x02)
    {
        EVE_Write8(0x00);
        EVE_Write8(0x00);
    }
    else if (BitmapDataSize == 0x01)
    {
        EVE_Write8(0x00);
        EVE_Write8(0x00);
        EVE_Write8(0x00);
    }

    LCD_CShigh();                                                               // CS high after burst write of image data

        // ------------ Now create screen to display image -------------

    MCU_Delay_20ms();

    LCD_CSlow();                                                                // CS low begins SPI transaction
    EVE_AddrForWr(RAM_CMD + cmdOffset);                                         // Send address to which first value will be written

    EVE_Write32(CMD_DLSTART);                                                   // Co-pro starts new DL at RAM_DL + 0
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Keep count of bytes sent so that write pointer can be updated at end
                                                                                // Keeping CS low and FT8xx will auto increment address for 'burst write'

    /*probar con 5*/
    EVE_Write32(BITMAP_HANDLE(0));                                              // Set bitmap handle
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(BITMAP_SOURCE(0));                                              // Bitmap data starts at 0
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(BITMAP_LAYOUT(RGB565,960, LCD_VSIZE));                                  // Tell FT8xx about the properties of the image data
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(BITMAP_SIZE(NEAREST, BORDER, BORDER, LCD_HSIZE, LCD_VSIZE));                   // Tell FT8xx about the on-screen properties of the image
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    /*intentar quitando esto*/
    EVE_Write32(BEGIN(BITMAPS));                                                // Begin drawing bitmaps
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(VERTEX2F(0,0));                                             // Draw at (100,100)
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(END());                                                         // End drawing images
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(DISPLAY());                                                     // Instruct the graphics processor to show the list
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(CMD_SWAP);                                                      // Make this list active
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    LCD_CShigh();                                                               // Chip Select high concludes burst

    EVE_MemWrite32(REG_CMD_WRITE, (cmdOffset));                                 // Update the ring buffer pointer
                                                                                // Co-processor will now execute all of the above commands and create a display list
    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Await completion of processing and record starting address for next screen update

    MCU_Delay_500ms();
    MCU_Delay_500ms();
}


void APP_ConvertedBitmap(void)
{
    uint8_t i= 0;

    uint32_t DataPointer = 0;
    uint32_t DataSize = 261120;


    // ------------ Load image data -------------

    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty

    while(1)
    {
        //printf("lcd image from uart %d \n\r", UpdImage);
        DataPointer = 0;
        APP_ReadImagefromSD(UpdImage);

        LCD_CSlow();                                                                // CS low begins SPI transaction
        EVE_AddrForWr(RAM_G);                                                       // Send address to which first value will be written

        while(DataPointer < DataSize)
        {
            EVE_Write8(data_buffer[DataPointer]);                                   // Send data byte-by-byte from array
            DataPointer++;
        }
        //index++;

        LCD_CShigh();                                                               // CS high after burst write of image data

            // ------------ Now create screen to display image -------------

        MCU_Delay_20ms();


        LCD_CSlow();                                                                // CS low begins SPI transaction
        EVE_AddrForWr(RAM_CMD + cmdOffset);                                         // Send address to which first value will be written

        EVE_Write32(CMD_DLSTART);                                                   // Co-pro starts new DL at RAM_DL + 0
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Keep count of bytes sent so that write pointer can be updated at end
                                                                                    // Keeping CS low and FT8xx will auto increment address for 'burst write'

        EVE_Write32(BEGIN(BITMAPS));                                                // Begin drawing bitmaps
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(VERTEX2F(0,0));                                                 // Draw at (0,0)
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(END());                                                         // End drawing images
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(DISPLAY());                                                     // Instruct the graphics processor to show the list
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(CMD_SWAP);                                                      // Make this list active
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        LCD_CShigh();                                                               // Chip Select high concludes burst

        EVE_MemWrite32(REG_CMD_WRITE, (cmdOffset));                                 // Update the ring buffer pointer
                                                                                    // Co-processor will now execute all of the above commands and create a display list
        cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Await completion of processing and record starting address for next screen update
    }
}


void APP_DigitsFont(void)
{
    uint16_t DataOffset = 1000;
    uint16_t DataPointer = 0;
    uint16_t DataSize = 7273; //(148+7125))
    uint16_t BitmapDataSize = 0;

    // ------------------------- Load image data -------------------------------

    // Load the image data from the MCU's Flash into the RAM_G of the FT81x
    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty

    LCD_CSlow();                                                                // CS low begins SPI transaction
    EVE_AddrForWr(RAM_G+DataOffset);                                            // Send address to which first value will be written

    while(DataPointer < DataSize)                                               // Stream data
    {
        EVE_Write8(MetricBlock[(DataPointer)]);
        DataPointer ++;
    }

   // Add 3, 2 or 1 bytes padding to make it  a multiple of 4 bytes
    BitmapDataSize = DataSize & 0x03;                                           // Mask off the bottom 2 bits

    if (BitmapDataSize == 0x03)
    {
        EVE_Write8(0x00);
    }
    else if (BitmapDataSize == 0x02)
    {
        EVE_Write8(0x00);
        EVE_Write8(0x00);
    }
    else if (BitmapDataSize == 0x01)
    {
        EVE_Write8(0x00);
        EVE_Write8(0x00);
        EVE_Write8(0x00);
    }

    LCD_CShigh();

    MCU_Delay_20ms();

    // ---------- Now create a screen with text written in this font -----------

    LCD_CSlow();                                                                // Begin a new SPI burst by setting CS low and sending
    EVE_AddrForWr(RAM_CMD + cmdOffset);                                         // first address to be written

    EVE_Write32(CMD_DLSTART);                                                   // DL_Start command tells co-pro to begin new DL
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the byte count

    EVE_Write32(CLEAR_COLOR_RGB(0,0,0));                                        // Set the default clear color to black
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    EVE_Write32(CLEAR(1,1,1));                                                  // Clear the screen Attributes color, stencil and tag buffers
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    EVE_Write32(BITMAP_HANDLE(14));                                             // Set the bitmap properties for handle 14
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    EVE_Write32(BITMAP_SOURCE(-1252));                                          // This value can be found in font converter output data
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    EVE_Write32(BITMAP_LAYOUT(L1,3,25));                                        // Specify the format, linestride, height
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    EVE_Write32(BITMAP_SIZE(NEAREST, BORDER, BORDER, 18,25));                   // Specify bitmap parameters
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    EVE_Write32(CMD_SETFONT);                                                   // Set Font
    EVE_Write32(14);                                                            // Font handle
    EVE_Write32 (1000);                                                         // Address of data in RAM_G
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 12);                                // Update the command pointer

    EVE_Write32 (COLOR_RGB(255,255,255));                                       // Specify colour of text
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer, must be multiple of 4

    EVE_Write32 (CMD_TEXT);                                                     // command text 0xFFFFFF0C
    EVE_Write16 (100);                                                          // x
    EVE_Write16 (200);                                                          // y
    EVE_Write16 (14);                                                           // font
    EVE_Write16 (0);                                                            // options
    EVE_Write8 (0x30);                                                          // string H
    EVE_Write8 (0x31);                                                          // string E
    EVE_Write8 (0x32);                                                          // string L
    EVE_Write8 (0x33);                                                          // string L
    EVE_Write8 (0x34);                                                          // string O
    EVE_Write8 (0);                                                             // null
    EVE_Write8 (0);                                                             // padding
    EVE_Write8 (0);                                                             // padding since overall command not multiple of 4
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 20);                                // Update the command pointer, must be multiple of 4

    EVE_Write32(DISPLAY());                                                     // Instruct the graphics processor to show the list
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    EVE_Write32(CMD_SWAP);                                                      // Make this list active
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Update the command pointer

    LCD_CShigh();                                                               // Chip select high

    EVE_MemWrite32(REG_CMD_WRITE, (cmdOffset));                                 // Update the CMD FIFO pointer

    cmdOffset = EVE_WaitCmdFifoEmpty();

    //APP_SnapShot2();

   /* while(1)
    {
    }
    */
}

// ############################ TOUCH CALIBRATION ##############################

void APP_Calibrate(void)
{
    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty and record current position in FIFO

    LCD_CSlow();                                                                // CS low begins SPI transaction
    EVE_AddrForWr(RAM_CMD + cmdOffset);                                         // Send address to which first value will be written

    EVE_Write32(CMD_DLSTART);                                                   // Co-pro starts new DL at RAM_DL + 0
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                                 // Keep count of bytes sent so that write pointer can be updated at end
                                                                                // Keeping CS low and FT8xx will auto increment address for 'burst write'
    EVE_Write32(CLEAR_COLOR_RGB(0,0,0));                                        // Set the default clear color to black
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(CLEAR(1,1,1));                                                  // Clear the screen Attributes color, stencil and tag buffers
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(CMD_CALIBRATE);                                                 // Calibrate
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(DISPLAY());                                                     // Instruct the graphics processor to show the list
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    EVE_Write32(CMD_SWAP);                                                      // Make this list active
    cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

    LCD_CShigh();                                                               // CS high finishes SPI burst write. Data now loaded into co-pro's FIFO but not yet executed

    EVE_MemWrite32(REG_CMD_WRITE, (cmdOffset));                                 // Update circular buffer write pointer to tell FT8xx to execute commands...

    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // ... and wait until co-pro has completed executing them
    // Code will only reach past above line once user has touched the 3 points on the calibration screen
    // Code could store result of the calibration (i.e. read the six 32-bit values from registers REG_TOUCH_TRANSFORM_A to _F) in MCU EEPROM and re-load them on each power up
}

void APP_SliderandButton(void)
{
    uint16_t Button3D = 0;

    cmdOffset = EVE_WaitCmdFifoEmpty();                                         // Wait for command FIFO to be empty and record current position in FIFO

    while(1)
    {
        LCD_CSlow();                                                            // CS low begins SPI transaction
        EVE_AddrForWr(RAM_CMD + cmdOffset);                                     // Send address to which first value will be written

        EVE_Write32(CMD_DLSTART);                                               // Co-pro starts new DL at RAM_DL + 0
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);                             // Keep count of bytes sent so that write pointer can be updated at end
                                                                                // Keeping CS low and FT8xx will auto increment address for 'burst write'

        EVE_Write32(CLEAR_COLOR_RGB(0,0,0));                                    // Set the default clear color to black
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(CLEAR(1,1,1));                                              // Clear the screen Attributes color, stencil and tag buffers
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);


        EVE_Write32(TAG_MASK(1));                                               // Enable tagging
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);


        EVE_Write32(TAG(2));                                                    // Tag following items with tag '2'
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32 (CMD_BUTTON);                                               // command button 0xFFFFFF0D
        EVE_Write16 (400);                                                      // x
        EVE_Write16 (200);                                                      // y
        EVE_Write16 (80);                                                       // w
        EVE_Write16 (30);                                                       // h
        EVE_Write16 (26);                                                       // font
        EVE_Write16 (Button3D);                                                 // options 0 = 3D effect or 256 = flat
        EVE_Write8 (0x42);                                                      // string B
        EVE_Write8 (0x75);                                                      // string u
        EVE_Write8 (0x74);                                                      // string t
        EVE_Write8 (0x74);                                                      // string t
        EVE_Write8 (0x6F);                                                      // string o
        EVE_Write8 (0x6E);                                                      // string n
        EVE_Write8 (0);                                                         // null terminates string
        EVE_Write8 (0);                                                         // pad with extra zero to make multiple of 4 in total
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 24);                            // Update the command pointer, must be multiple of 4


        EVE_Write32(TAG(5));                                                    // Tag following items with tag '5'
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32 (CMD_SLIDER);                                               // command slider 0xFFFFFF10
        EVE_Write16 (0x00F0);                                                   // x
        EVE_Write16 (0x0028);                                                   // y
        EVE_Write16 (0x000F);                                                   // width
        EVE_Write16 (0x00B0);                                                   // height
        EVE_Write16 (0x0000);                                                   // options
        EVE_Write16 (SlideVal);                                                 // value (i.e. position of handle)
        EVE_Write16 (0x00FF);                                                   // range of slider
        EVE_Write16 (0x0000);                                                   // dummy
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 20);                            // Update the command pointer, must be multiple of 4

        EVE_Write32 (CMD_TRACK);                                                // command track 0xFFFFFF2C
        EVE_Write16 (0x00F0);                                                   // x
        EVE_Write16 (0x0028);                                                   // y
        EVE_Write16 (0x000F);                                                   // width
        EVE_Write16 (0x00B0);                                                   // height
        EVE_Write16 (0x0005);                                                   // tag the tracked area with 5
        EVE_Write16 (0x0000);                                                   // dummy
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 16);                            // Update the command pointer, must be multiple of 4



        EVE_Write32(TAG_MASK(0));                                               // Tag mask - disable tagging of any subsequent items
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(BEGIN(FTPOINTS));                                           // Begin drawing points
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(POINT_SIZE(point_size));                                    // Select the size of the dot to draw
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32 (COLOR_RGB(color,0,0));                                       // Colour = red or black (black == invisible since matches background colour)
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(VERTEX2F(440*16,100*16));                                   // Set the point center location (400+ half width of button))
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(END());                                                     // End the point
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(DISPLAY());                                                 // Instruct the graphics processor to show the list
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        EVE_Write32(CMD_SWAP);                                                  // Make this list active
        cmdOffset = EVE_IncCMDOffset(cmdOffset, 4);

        LCD_CShigh();                                                           // Chip Select high concludes burst

        EVE_MemWrite32(REG_CMD_WRITE, (cmdOffset));                             // Update the ring buffer pointer
                                                                                // Co-processor will now execute all of the above commands and create a display list
        cmdOffset = EVE_WaitCmdFifoEmpty();                                     // Await completion of processing and record starting address for next screen update

        // ------ read tag and tracker values --------
        TagVal = EVE_MemRead8(REG_TOUCH_TAG);                                   // Get Tag value
        TrackerVal = EVE_MemRead32(REG_TRACKER);                                // Read the value of the tag and track register

        if(TagVal == 2)
        {// If button pushed tag register will read 2
            color = 0xFF;                                                       // change red amount to 255
            Button3D = 256;
        }
        else
        {// Otherwise...
            color = 0x00;                                                       // change red amount to 0
            Button3D = 0;
        }

        if(TagVal == 5)                                                         // if slider touched...
        {
            SlideVal = (TrackerVal >> 24);                                      // ... then get the tracker value.
            // Note: Value of tracking is 16 bits but we only want upper 8 bits since the slider is set for 8 bit resolution
        }

        //APP_SnapShot2();

        // Result of button press and slider value will be displayed next time round
    }
}
