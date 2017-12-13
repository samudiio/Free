#include "Afec_Ctrl.h"

Afec *MyAfec;

uint16_t LOC_SIZE;
uint32_t LOC_SAMP_PER;
uint32_t *LOC_BUFF_ADDR; 
uint32_t *my_argument;

AfeDma AfeDmaCfg;
sXdmad my_sXdmad; 


void My_AfeCallback( uint8_t a, void* b ){  
    printf("Secuencia terminada");
}


void SET_AFEC_SAMPLING(uint16_t Size,uint32_t SampPer, uint32_t *BuffAddr ){  
     
  LOC_SIZE     = Size;
  LOC_SAMP_PER = SampPer;
  LOC_BUFF_ADDR = BuffAddr; 
  
  AFEC_Init(); 
  
  AfeCmd My_AfeCmd = {LOC_BUFF_ADDR,LOC_SIZE,My_AfeCallback,my_argument};    
  my_sXdmad.pXdmacs = XDMAC; 
  
  Afe_ConfigureDma(&AfeDmaCfg,AFEC0,ID_AFEC0,&my_sXdmad);
  Afe_SendData(&AfeDmaCfg,&My_AfeCmd);    
     
  Timer0_Init(LOC_SAMP_PER);   
}




void AFEC_Init(void){
        
  AFEC_Initialize(AFEC0,ID_AFEC0 );     //PMC Enable
  AFEC_SetAnalogControl(AFEC0,(AFEC_ACR_PGA0EN|AFEC_ACR_PGA1EN|AFEC_ACR_IBCTL(1))); //Enable Gain amplifiers
  AFEC_SetClock(AFEC0,AFE_CLK,BOARD_MCK); //Set preescaler
  //Se configura el Mode register para que la interrupcion se haga por el TA1 del TMC0
  AFEC0->AFEC_MR |= 
    AFEC_MR_TRANSFER(2)|
    AFEC_MR_TRGEN_EN   |
    AFEC_MR_TRGSEL_AFEC_TRIG1|
    AFEC_MR_SLEEP_NORMAL|AFEC_MR_FWUP_OFF|
    AFEC_MR_FREERUN_OFF|
    AFEC_MR_STARTUP_SUT64|
    AFEC_MR_ONE|
    AFEC_MR_USEQ_NUM_ORDER;    
    
  AFEC_EnableChannel(AFEC0, TEST_CHANNEL);
  AFEC_SetAnalogOffset(AFEC0, TEST_CHANNEL, CHANNEL_OFFSET); 
  AFEC_SetExtModeReg(AFEC0,0| AFEC_EMR_RES(256)| AFEC_EMR_TAG |AFEC_EMR_STM );
                                  
 /* NVIC_ClearPendingIRQ(AFEC0_IRQn);
  NVIC_EnableIRQ(AFEC0_IRQn);
  NVIC_SetPriority(AFEC0_IRQn ,3);                   
  AFEC_EnableIt(AFEC0,AFEC_IER_EOC5);
   */
}

void AFEC_SoftwareConversion(void){ //Activacion por software
  
  uint32_t ch,voltage; 
  AFEC_StartConversion(AFEC0);
  
  while (!(AFEC_GetStatus(AFEC0) & AFEC_ISR_EOC5));
         
  voltage = ((AFEC_GetConvertedData(AFEC0,TEST_CHANNEL) & AFEC_LCDR_LDATA_Msk)) *3254/ 4096;
  printf("CH 05 - Voltage = %04u (mV) \n\r" ,(unsigned int)voltage);
    
}



void AFEC0_Handler (void){
  /*uint32_t voltage;
  uint32_t raw_voltage;
  raw_voltage = AFEC_GetConvertedData(AFEC0,TEST_CHANNEL);
  voltage = ((raw_voltage & AFEC_LCDR_LDATA_Msk)) *3254/ 4096;
  printf("CH 05 - Voltage = %04u (mV) --> %#010x \n\r" ,(unsigned int)voltage,raw_voltage);
*/
}



void XDMAC_Handler(void)
{
	          printf("Secuencia terminada");
}



