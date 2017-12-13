/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"
#include "app_scheduler.h"
#include "Tasks.h"
#include "Mem_Alloc.h"
#include "Afec_Ctrl.h"
#include <stdbool.h>
#include <stdio.h>
/**  Fast fourier transform */
#include "fft.h"
#include "ecg_data.h"

#include "arm_math.h"
#include "arm_const_structs.h"

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/
/*~~~~~~  Local definitions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define TEST_LENGTH_SAMPLES 2048

#define BLOCK_SIZE            32
#define NUM_TAPS              9

/*~~~~~~  Global variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/** Auxiliary input buffer to accomodate data as FFT function expects it */
float       fft_inputData[TEST_LENGTH_SAMPLES];
/** Output magnitude data */
float       fft_signalPower[TEST_LENGTH_SAMPLES/2];
/** Auxiliary output variable that holds the frequency bin with the highest level of signal power */
uint32_t    u32fft_maxPowerIndex;
/** Auxiliary output variable that holds the maximum level of signal power */
float       fft_maxPower;


TaskType Tasks[]={
/*  TaskPriority    TaskId   TaskFunctionPointer   */
  {      5,        TASK_1MS,       vfnTsk_1ms    },
  {      4,        TASK_2MSA,      vfnTsk_2msA   },
  {      4,        TASK_2MSB,      vfnTsk_2msB   },
  {      3,        TASK_10MS,      vfnTsk_10ms   },
  {      2,        TASK_50MS,      vfnTsk_50ms   },
  {      1,        TASK_100MS,     vfnTsk_100ms  }
};

extern float ecg_resampled[2048];
extern float testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float testOutput[TEST_LENGTH_SAMPLES/2];

Mem_ReturnType ptr_a,ptr_b,ptr_c,ptr_d;

/* -------------------------------------------------------------------
 * Declare State buffer of size (numTaps + blockSize - 1)
 * ------------------------------------------------------------------- */

static float firStateF32[BLOCK_SIZE + NUM_TAPS - 1];

/* ----------------------------------------------------------------------
** FIR Coefficients buffer generated using fir1() MATLAB function.
** fir1(28, 6/24)
** ------------------------------------------------------------------- */

const float32_t firCoeffs32[NUM_TAPS] = {
  -0.0018225230f, -0.0015879294f, +0.0000000000f,
  +0.0036977508f, +0.0080754303f, +0.0085302217f,
  -0.0000000000f, -0.0173976984f, -0.0341458607f
};

/* ------------------------------------------------------------------
 * Global variables for FIR LPF Example
 * ------------------------------------------------------------------- */

uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;

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
uint32_t *BUFF_ADDR;
uint16_t size = 512;
uint32_t SAMP_PER = 100;
float ecg_resampled2[512];

extern int main( void )
{
    /** Auxiliary array index */
    uint16_t u16index;

    uint32_t *BuffAdd;
    float factor;
    float temp;
    float *inputF32, *outputF32;
    arm_fir_instance_f32 S;

    /* Initialize output buffer pointers */
    outputF32 = &testOutput[0];

	/* Disable watchdog */
	WDT_Disable( WDT ) ;

	/* Enable I and D cache */
	SCB_EnableICache();
	SCB_EnableDCache();

    Mem_Init();

	_ConfigureLeds() ;
  
  	/* Initialize Task Scheduler */
	vfnScheduler_Init(&Tasks[0]);
	/* Start execution of task scheduler */
	vfnScheduler_Start();
   /* Enable Floating Point Unit */
    vfnFpu_enable();
    /* Initialize AFEC */
    AFEC_Init();
  
    /* Call FIR init function to initialize the instance structure. */
    arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], blockSize);
  
    BUFF_ADDR = (uint32_t *)Mem_Alloc(size*sizeof(uint32_t));
  
    SET_AFEC_SAMPLING(size,SAMP_PER,BUFF_ADDR);

    BuffAdd = BUFF_ADDR;

    for(u16index = 0; u16index < size; u16index++)
    {
        factor = 3.3/1024;
        temp = *(BuffAdd + u16index*4);
        ecg_resampled2[u16index] = temp*factor;
    }

    /*Prepare data for FFT operation */
    for (u16index = 0; u16index < (TEST_LENGTH_SAMPLES/2); u16index++)
    {
        fft_inputData[(2*u16index)] = ecg_resampled2[u16index];
        fft_inputData[(2*u16index) + 1] = 0;
    }
    /** Perform FFT on the input signal */
    fft(fft_inputData, fft_signalPower, TEST_LENGTH_SAMPLES/2, &u32fft_maxPowerIndex, &fft_maxPower);

    inputF32 = &fft_signalPower[0];

    for(u16index =0; u16index < numBlocks; u16index++)
    {
        arm_fir_f32(&S, inputF32 + (u16index * blockSize), outputF32 + (u16index * blockSize), blockSize);
    }

    /* Publish through emulated Serial the byte that was previously sent through the regular Serial channel */
    printf("fft_maxPowerIndex , %5d  fft_maxPower %5.4f \r\n", u32fft_maxPowerIndex, fft_maxPower);
    printf("testOutput %5.4f \r\n", testOutput[0]);

	/*-- Loop through all the periodic tasks from Task Scheduler --*/
	for(;;)
	{
		/* Perform all scheduled tasks */    
		vfnTask_Scheduler();
	}

}
