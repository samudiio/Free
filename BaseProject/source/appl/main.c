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
//#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
//#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
//#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
//#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)
//
//extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName);
//extern void vApplicationIdleHook(void);
//extern void vApplicationTickHook(void);
//extern void vApplicationMallocFailedHook(void);
//extern void xPortSysTickHandler(void);

/* Set mainCREATE_SIMPLE_BLINKY_DEMO_ONLY to one to run the simple blinky demo,
or 0 to run the more comprehensive test and demo application. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY  BLINKY


///**
// * \brief Called if stack overflow during execution
// */
//extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
//		signed char *pcTaskName)
//{
//	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
//	/* If the parameters have been corrupted then inspect pxCurrentTCB to
//	 * identify which task has overflowed its stack.
//	 */
//	for (;;) {
//	}
//}
//
///**
// * \brief This function is called by FreeRTOS idle task
// */
//extern void vApplicationIdleHook(void)
//{
//}
//
///**
// * \brief This function is called by FreeRTOS each tick
// */
//extern void vApplicationTickHook(void)
//{
//}
//
//extern void vApplicationMallocFailedHook(void)
//{
//	/* Called if a call to pvPortMalloc() fails because there is insufficient
//	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
//	internally by FreeRTOS API functions that create tasks, queues, software
//	timers, and semaphores.  The size of the FreeRTOS heap is set by the
//	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
//
//	/* Force an assert. */
//	configASSERT( ( volatile void * ) NULL );
//}
//
///**
// * \brief This task, when activated, send every ten seconds on debug UART
// * the whole report of free heap and total tasks status
// */
//static void task_monitor(void *pvParameters)
//{
//	static portCHAR szList[256];
//	UNUSED(pvParameters);
//
//	for (;;) {
//		printf("--- Number of active tasks ## %u\n\r", (unsigned int)uxTaskGetNumberOfTasks());
//		vTaskList((signed portCHAR *)szList);
//		printf(szList);
//		vTaskDelay(1000);
//	}
//}
//
///**
// * \brief This task, when activated, make LED blink at a fixed rate
// */
//static void task_led(void *pvParameters)
//{
//    UNUSED(pvParameters);
//    for (;;)
//    {
//        LED_Toggle(0);
//        vTaskDelay(100);
//    }
//}

/*-------------------- BLINKY DEMO Test ------------*/
/*
 * Configure the hardware as necessary to run this demo.
 */
static void prvSetupHardware( void );

/*
 * main_blinky() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 * main_full() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 0.
 */
#if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 1
    extern void main_blinky( void );
#else
    extern void main_full( void );
#endif /* #if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 1 */

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/*~~~~~~  Global variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/

/**
 *  \brief Configure LEDs
 *
 *  Configures LEDs \#1 and \#2 (cleared by default).
 */
static void _ConfigureLeds( void )
{
	LED_Configure( 0 ) ;
	LED_Configure( 1 ) ;
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
/**
 *  \brief getting-started Application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */

extern int main( void )
{
    /* Configure the hardware ready to run the demo. */
        prvSetupHardware();
	
    /* Disable watchdog */
    WDT_Disable( WDT ) ;

    /* Enable I and D cache */
    SCB_EnableICache();
    SCB_EnableDCache();

    _ConfigureLeds() ;

    /* Enable Floating Point Unit */
    vfnFpu_enable();

        /*Free RTOS Example*/

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


/*Free RTOS Example*/

//    /* Create task to monitor processor activity */
//    if (xTaskCreate(task_monitor, "Task Monitor", TASK_MONITOR_STACK_SIZE, NULL, TASK_MONITOR_STACK_PRIORITY, NULL) != pdPASS)
//    {
//        printf("Failed to create Monitor task\r\n");
//    }
//
//    /* Create task to make led blink */
//    if (xTaskCreate(task_led, "Led 0", TASK_LED_STACK_SIZE, NULL, TASK_LED_STACK_PRIORITY, NULL) != pdPASS)
//    {
//        printf("Failed to create test led task\r\n");
//    }
//
//    /* Start the scheduler. */
//    vTaskStartScheduler();
//
//    /* Will only get here if there was insufficient memory to create the idle task. */
//    return 0;
}

static void prvSetupHardware( void )
{
    /* Disable watchdog. */
    WDT_Disable( WDT );
//  WDT_Disable( WDT1 );

    SCB_EnableICache();
//  SCB_EnableDCache();

    LED_Configure( 0 );
    LED_Configure( 1 );
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
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
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
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
/*-----------------------------------------------------------*/

/* Just to keep the linker happy. */
int __write( int x );
int __write( int x )
{
    return x;
}

