/*******************************************************************************

Copyright (c) Future Technology Devices International

THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

FTDI DRIVERS MAY BE USED ONLY IN CONJUNCTION WITH PRODUCTS BASED ON FTDI PARTS.

FTDI DRIVERS MAY BE DISTRIBUTED IN ANY FORM AS LONG AS LICENSE INFORMATION IS NOT MODIFIED.

IF A CUSTOM VENDOR ID AND/OR PRODUCT ID OR DESCRIPTION STRING ARE USED, IT IS THE
RESPONSIBILITY OF THE PRODUCT MANUFACTURER TO MAINTAIN ANY CHANGES AND SUBSEQUENT WHQL
RE-CERTIFICATION AS A RESULT OF MAKING THESE CHANGES.

********************************************************************************

This code is provided as an example only and is not guaranteed by FTDI/BridgeTek.
FTDI/BridgeTek accept no responsibility for any issues resulting from its use.
By using this code, the developer of the final application incorporating any 
parts of this sample project agrees to take full responsible for ensuring its 
safe and correct operation and for any consequences resulting from its use.

Revision History: 
 
Version Author              Date        Description
******* ******************* *********** ****************************************
1.0     G Brown             Feb 2017    Initial version
 
*/



#include <xc.h>
#include "Library.h" 
#include "FT8xx.h"

#define _XTAL_FREQ 12000000      // Required for _delay() function

// PIC configuration definitions
#pragma config WDTEN = OFF                                                      // Watchdog Timer Enable bit 
#pragma config PWRTEN = OFF                                                     // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = EXTMCLR                                                  // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP0 = OFF                                                        // Code Protection Block 0 
#pragma config CP1 = OFF                                                        // Code Protection Block 1 
#pragma config CP2 = OFF                                                        // Code Protection Block 2 
#pragma config CP3 = OFF                                                        // Code Protection Block 3 
#pragma config CPB = OFF                                                        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF                                                        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)
#pragma config BOREN = ON                                                       // Brown-out Reset Selection bits (BOR enabled)

#pragma config IESO = ON                                                        // Internal External Switchover enable bit 
#pragma config FCMEN = ON                                                       // Fail-Safe Clock Monitor Enabled bit 
#pragma config PRICLKEN = ON                                                    // 
#pragma config PLLCFG = ON                                                      // 
#pragma config FOSC = 0x03                                                      // 


//############################################################################## 
//##############################################################################
//                                MCU LAYER 
//##############################################################################  
//##############################################################################
   
// This is the MCU specific section and contains the functions which talk to the
// PIC registers. If porting the code to a different PIC or to another MCU, these
// should be modified to suit the registers of the selected MCU.

// ------------------- MCU specific initialisation  ----------------------------
void MCU_Init(void)
{
    ANSELA = 0x00;      // all digital IO
    ANSELB = 0x00;      // all digital IO
    ANSELC = 0x00;      // all digital IO
    ANSELD = 0x00;      // all digital IO
        
    // Port pin set-up
    TRISCbits.TRISC7 = 0;                                                       // CS
    TRISCbits.TRISC6 = 0;                                                       // PD pin
    TRISCbits.TRISC3 = 0;                                                       // SCK 
    TRISCbits.TRISC4 = 1;                                                       // SDI (MISO)
    TRISCbits.TRISC5 = 0;                                                       // SDO (MOSI) 
    
                                                                                /***  UART Pins ***/
    TRISDbits.TRISD7 = 1;                                                       // RXD INPUT
    TRISDbits.TRISD6 = 0;                                                       // TXD OUTPUT
    TRISDbits.TRISD5 = 1;                                                       // RTS INPUT (FROM FT232 RTS))
    TRISDbits.TRISD4 = 0;                                                       // CTS OUTPUT (TO FT232 CTS)    
       
    LATBbits.LATB0 = 1;     //??? B0 is not connected ??
    
    // SPI 1 set-up
    SSP1CON1bits.SSPEN  = 0;                                                    // Disable SPI1 and configure it
    
    SSP1CON1bits.WCOL   = 0;    //Write Collision Detect bit.       0 = No colision                   Read
    SSP1CON1bits.SSPOV  = 0;    //Receive Overflow Indicator bit.   0 = No overflow                 Read            in SAM - OVRES read reg
    SSP1CON1bits.CKP    = 0;    //Clock Polarity Select bit.        0 = Idle state for clock is a low level

    SSP1CON1bits.SSPM3 = 0;     //SSPM<3:0> Synchronous Serial Port Mode select bits
    SSP1CON1bits.SSPM2 = 0;     //0001 = SPI Master mode, clock = FOSC/16
    SSP1CON1bits.SSPM1 = 0;
    SSP1CON1bits.SSPM0 = 1;

    /**** NOTE by Samuel
     * A 12MHz crystal was used and the internal x4 PLL enabled to run the device at 48MHz.
     * SSPxM<3:0>: Synchronous Serial Port Mode Select bits
     * 0001 = SPI Master mode, clock = FOSC/16
     * clock = 48Mhz/16
     * clock = 3Mhz
     */

    SSP1STATbits.SMP = 0;   //SPI Data Input Sample bit.            0 = Input data sampled at middle of data output time
    SSP1STATbits.CKE = 1;   //SPI Clock Edge Select bit.            1 = Transmit occurs on transition from active to idle clock state
    SSP1STATbits.BF  = 0;   //Buffer Full Status bit.               0 = Receive not complete, SSPxBUF is empty      Read bit

    SSP1CON1bits.SSPEN  = 1;                                                    // Enable SPI1 after configuration
  
}

// ########################### GPIO CONTROL ####################################

// --------------------- Chip Select line low ----------------------------------
void MCU_CSlow(void)
{
    LATCbits.LATC7 = 0;                                                         // CS# line low
    Nop();
}  

// --------------------- Chip Select line high ---------------------------------
void MCU_CShigh(void)
{
    Nop();
    LATCbits.LATC7 = 1;                                                         // CS# line high
}

// -------------------------- PD line low --------------------------------------
void MCU_PDlow(void)
{
    LATCbits.LATC6 = 0;                                                         // PD# line low
}

// ------------------------- PD line high --------------------------------------
void MCU_PDhigh(void)
{
    LATCbits.LATC6 = 1;                                                         // PD# line high
}

// ################################# SPI #######################################

// --------------------- SPI Send and Receive ----------------------------------
unsigned char MCU_SPIReadWrite(unsigned char DataToWrite)
{
    unsigned char DataRead = 0;
    
    SSP1BUF = (DataToWrite);                            // Write data to SPI data register
    while(!SSP1STATbits.BF);                            // Wait for completion of the SPI transfer
    DataRead = SSP1BUF;                                 // Get the value clocked in from the FT8xx
    
    return DataRead;
}

void MCU_UART_Init(void)
{
    TXSTA2bits.SYNC = 0;                                                        // Asynchronous
    RCSTA2bits.SPEN = 1;                                                        // enable serial port pins 
    RCSTA2bits.CREN = 1;                                                        // Enable reception 
    RCSTA2bits.SREN = 0;                                                        // No effect 
    PIE3bits.TX2IE = 0;                                                         // Disable tx interrupts 
    PIE3bits.RC2IE = 0;                                                         // Disable rx interrupts 
    TXSTA2bits.TX9 = 0;                                                         // 8- or 9-bit transmission 
    RCSTA2bits.RX9 = 0;                                                         // 8- or 9-bit reception
    TXSTA2bits.TXEN = 1;                                                        // Enable the transmitter
    TXSTA2bits.BRGH2 = 0;                                                       //
    BAUDCON2bits.BRG16 = 0;   
    
    // Baud rate = 48,000,000/(64(n+1))
    // 48000000 / (64(12+1)) = 57692 baud  
    SPBRG2 = 0x0C;
    
    // Baud rate = 48,000,000/(64(n+1))
    // 48000000 / (64(2+1)) = 250000 baud 
    //SPBRG2 = 0x02;
}

void MCU_UART_Tx(unsigned char SerialTxByte)
{
    unsigned char PortValue = 0;
    
 // PortValue = PORTD;
 // while((PortValue & 0x20) == 0x20)
 // {
 //       PortValue = PORTD;
 // }

  while (!PIR3bits.TX2IF)
  {
  }
  TXREG2 = SerialTxByte;
}

unsigned char MCU_UART_Rx(void)
{
    unsigned char SerialRxByte = 0x00;
    
    while(!PIR3bits.RC2IF)
    {
    }
    SerialRxByte = RCREG2;
    
    return SerialRxByte;
    
}

void MCU_Delay_20ms(void)
{
    // 
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
    __delay_ms(20);                                                             // gives 20ms due to x4 PLL
    
    
    
}

void MCU_Delay_500ms(void)
{
    unsigned char dly = 0;                                                      
    
    for(dly =0; dly < 100; dly++)
    {
    __delay_ms(20);
    }
}

