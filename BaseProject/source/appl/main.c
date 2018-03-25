/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*SD/MMC Card*/
#include "ctrl_access.h"
#include "ff.h"


#include "lcd_app.h"

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


#define TOTAL_SIZE 261120

/*~~~~~~  Global variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Read buffer */
    static uint8_t data_buffer[TOTAL_SIZE];

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware( void )
{
    /* Disable watchdog. */
    WDT_Disable( WDT );

    /* Enable I and D cache */
    SCB_EnableICache();
    SCB_EnableDCache();




    /*LDC_Init();
    APP_Init();
    LCD_ChangeClock();
   */

    //APP_Calibrate();                                                          // NOTE:  Enable if using any touch demos

        // Important Note: Enable only one demo below at a time.
        // If running any of the following demos, un-comment the APP_Calibrate function above
        //     * APP_SliderAndButton


    //APP_FlashingDot();    //Creating a basic screen via co-pro
    //APP_VertexTranslate();
    //APP_Text();

/******* Show Image ********/
    //APP_ConvertedBitmap_FirstTime();
    //APP_ConvertedBitmap();
	
	    /********           ********/

    //APP_DigitsFont();
    //APP_SliderandButton();

    /*  Configures LEDs \#1 and \#2 (cleared by default). */
    LED_Configure( 0 );
    LED_Configure( 1 );

    /* Enable Floating Point Unit */
    //vfnFpu_enable();
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
    char salomon_file_name[] = "0:salomon.raw";
    char test_file_name[] = "0:sd_mmc_my_test.txt";
    uint32_t     i;
    char   *getstatus;

    Ctrl_status status;
    FRESULT res;
    FATFS fs;
    FIL file_object;

    /* Configure the hardware ready to run the demo. */
    prvSetupHardware();

    /* Initialize SD MMC stack */
    sd_mmc_init();


    printf("\x0C\n\r-- SD/MMC/SDIO Card Example on FatFs --\n\r");

    while (1) {
    printf("Please plug an SD, MMC or SDIO card in slot.\n\r");

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

    printf("\n\rMount disk (f_mount)...\r\n");
    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
    if (FR_INVALID_DRIVE == res) {
        printf("[FAIL] res %d\r\n", res);
        goto main_end_of_test;
    }
    printf("\n\r[OK]\r\n");

//      printf("\n\rCreate a file (f_open)...\r\n");
//      test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
//      res = f_open(&file_object,
//              (char const *)test_file_name,
//              FA_CREATE_ALWAYS | FA_WRITE);
//      if (res != FR_OK) {
//          printf("[FAIL] res %d\r\n", res);
//          goto main_end_of_test;
//      }
//      printf("\n\r[OK]\r\n");
//
//      printf("\n\rWrite to test file (f_puts)...\r\n");
//      if (0 == f_puts("test SD/MMC stack LCD\n", &file_object)) {
//          f_close(&file_object);
//          printf("[FAIL]\r\n");
//          goto main_end_of_test;
//      }
//      printf("\n\r[OK]\r\n");
//      f_close(&file_object);


    /***** Read My file ****/

    /* Open the file */
    printf("Open file (f_open)...\r\n");
    res = f_open(&file_object, (char const *)salomon_file_name, FA_OPEN_EXISTING | FA_READ);
    if (res != FR_OK) {
        printf("[FAIL] res %d\r\n", res);
        goto main_end_of_test;
    }

    /* Read file */
    printf("Read file (f_read)...\r\n");
    memset(data_buffer, 0, TOTAL_SIZE);

    getstatus = f_gets(data_buffer, TOTAL_SIZE, &file_object);
    if (getstatus != NULL) {
        printf("Read file [OK]\r\n");
    }

    for (i = TOTAL_SIZE-10; i < TOTAL_SIZE; i++)
    {
        printf("Data[%d] = %d\r\n", i, data_buffer[i]);
    }


    /* Close the file*/
    printf("Close file (f_close)...\r\n");
    f_close(&file_object);


    printf("Test is successful.\n\r");

main_end_of_test:
    printf("Please unplug the card.\n\r");
    while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
    }
}









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
        main_full();
    }
    #endif

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

