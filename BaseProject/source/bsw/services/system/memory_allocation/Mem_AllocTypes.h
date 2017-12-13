/*******************************************************************************/
/**
\file       Mem_AllocTypes.h
\author     Juan Campeche
\version    0.1
\date       20/09/2017
*/
#ifndef _MEM_ALLOCTYPES_
#define _MEM_ALLOCTYPES_

/*-- Includes ----------------------------------------------------------------*/
#include "board.h"
#include <stdint.h>
#include "app_scheduler.h"


typedef uint32_t *Mem_ReturnType;  //debe regresar 32 bits no 8 bits 
typedef uint32_t *Mem_Uint8PtrType; //definir si es puntero o no *
typedef uint32_t Mem_SizeType;

typedef struct  
{
  Mem_Uint8PtrType memStart;
  Mem_Uint8PtrType memEnd;
  Mem_Uint8PtrType currAddr;
  Mem_Uint8PtrType oldAddr ;
  Mem_SizeType freebytes;
}MemHandlerType;


/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/


#endif 