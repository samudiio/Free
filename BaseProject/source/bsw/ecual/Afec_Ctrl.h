#ifndef _AFEC_CTRL_
#define _AFEC_CTRL_


//#include "afe_dma.h"
#include "board.h"
#include "afec.h"
#include "afe_dma.h"


#define AFE_CLK 2200000
#define TEST_CHANNEL 5
#define CHANNEL_OFFSET 0x200

void SET_AFEC_SAMPLING(uint16_t Size,uint32_t SampPer,uint32_t *BuffAddr);

void AFEC_Init(void);

void AFEC_SoftwareConversion(void);

#endif