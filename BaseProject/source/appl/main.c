/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Standard demo includes. */
#include "TimerDemo.h"
#include "QueueOverwrite.h"
#include "EventGroupsDemo.h"
#include "IntSemTest.h"
#include "TaskNotify.h"

/* Library includes. */
#include "board.h"

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

/* Set mainCREATE_SIMPLE_BLINKY_DEMO_ONLY to one to run the simple blinky demo,
or 0 to run the more comprehensive test and demo application. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY  BLINKY

#define SPI0_CS3  3

/*----------------------------------------------------------------------------
 *        Local prototypes
 *----------------------------------------------------------------------------*/

/* Prototypes for the standard FreeRTOS callback/hook functions implemented within this file. */
extern void vApplicationMallocFailedHook(void);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
//extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName);

/*-------------------- BLINKY DEMO Test ------------*/
/*
 * main_blinky() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 * main_full() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 0.
 */
#if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 1
    extern void main_blinky( void );
#else
    extern void main_full( void );
#endif /* #if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 1 */


/*~~~~~~  Global variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/** Pins to configure for the application. */
static const Pin SPI_pins[] = {
    PIN_SPI_MISO,
    PIN_SPI_MOSI,
    PIN_SPI_SPCK,
    PIN_SPI_NPCS3
};

/** SPI Clock setting (Hz) */
static uint32_t spiClock = 500000;

uint8_t pTxBuffer[] = "This is SPI LoopBack Test Buffer";

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware( void )
{
    uint8_t i;

    /* Disable watchdog. */
    WDT_Disable( WDT );

    /* Enable I and D cache */
    SCB_EnableICache();
//  SCB_EnableDCache();


    /* Configure SPI pins*/
    PIO_Configure( SPI_pins, PIO_LISTSIZE(SPI_pins) );

    SPI_Configure(SPI0, ID_SPI0, ( SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_PCS( SPI0_CS3 )));

    /*SPI_ConfigureNPCS( SPI0, SPI0_CS3,
                       SPI_DLYBCT( 1000, BOARD_MCK ) |
                       SPI_DLYBS(1000, BOARD_MCK) |
                       SPI_SCBR( spiClock, BOARD_MCK) );*/

    SPI_ConfigureNPCS(SPI0, SPI0_CS3,
                          SPI_CSR_CPOL | SPI_CSR_BITS_8_BIT |
                          SPI_DLYBCT(100, BOARD_MCK) |
                          SPI_DLYBS(6, BOARD_MCK) |
                          SPI_SCBR(20000000, BOARD_MCK));

    SPI_Enable(SPI0);

    for (i = 0; ;i++) {
        SPI_Write(SPI0, SPI0_CS3 , (uint16_t)pTxBuffer[i]);
        if (pTxBuffer[i] =='\0')
            break;
    }
    if (SPI_IsFinished(SPI0)) {
        SPI_Disable(SPI0);
    }


    /*  Configures LEDs \#1 and \#2 (cleared by default). */
    LED_Configure( 0 );
    LED_Configure( 1 );

    /* Enable Floating Point Unit */
    vfnFpu_enable();
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/**
 *  \brief Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
extern int main( void )
{
    /* Configure the hardware ready to run the demo. */
    prvSetupHardware();

    /*Free RTOS 8.2.1 Example*/

    printf("Free RTOS 8.2.1 Example \n\r");

    /* The mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is described at the top
    of this file. */
    #if( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 1 )
    {
        main_blinky();
    }
    #else
    {
        //main_full();
    }
    #endif

    while (1) {

    }

    return 0;
}

/*----------------------------------------------------------------------------
 *        Standard FreeRTOS callback/hook functions
 *----------------------------------------------------------------------------*/

extern void vApplicationMallocFailedHook(void)
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}

/**
 * \brief This function is called by FreeRTOS idle task
 */
extern void vApplicationIdleHook( void )
{
    volatile size_t xFreeHeapSpace;

    /* This is just a trivial example of an idle hook.  It is called on each
    cycle of the idle task.  It must *NOT* attempt to block.  In this case the
    idle task just queries the amount of FreeRTOS heap that remains.  See the
    memory management section on the http://www.FreeRTOS.org web site for memory
    management options.  If there is a lot of heap memory free then the
    configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
    RAM. */
    xFreeHeapSpace = xPortGetFreeHeapSize();

    /* Remove compiler warning about xFreeHeapSpace being set but never used. */
    ( void ) xFreeHeapSpace;
}

/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook( void )
{
    #if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 0
    {
        /* The full demo includes a software timer demo/test that requires
        prodding periodically from the tick interrupt. */
        vTimerPeriodicISRTests();

        /* Call the periodic queue overwrite from ISR demo. */
        vQueueOverwritePeriodicISRDemo();

        /* Call the periodic event group from ISR demo. */
        vPeriodicEventGroupsProcessing();

        /* Call the code that uses a mutex from an ISR. */
        vInterruptSemaphorePeriodicTest();

        /* Call the code that 'gives' a task notification from an ISR. */
        xNotifyTaskFromISR();
    }
    #endif
}

/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    /* If the parameters have been corrupted then inspect pxCurrentTCB to
     * identify which task has overflowed its stack.
     */
    printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}

