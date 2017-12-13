/*******************************************************************************/
/**
\file       Mem_Alloc.h
\author     Juan Campeche
\version    0.1
\date       11/09/2017
*/
#ifndef _MEM_ALLOC_
#define _MEM_ALLOC_

/*-- Includes ----------------------------------------------------------------*/
#include "Mem_AllocTypes.h"

extern uint32_t MEM_HEAP_START;
extern uint32_t MEM_HEAP_END;



/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
void Mem_Init(void);

Mem_ReturnType Mem_Alloc(Mem_SizeType);


#endif 