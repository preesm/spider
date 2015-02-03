/*
 * cache.c
 *
 *  Created on: 7 nov. 2013
 *      Author: kdesnos
 */


#include "cache.h"
#include <ti/csl/csl_xmcAux.h>
#include <ti/csl/src/intc/csl_intc.h>
#include <ti/csl/src/intc/csl_intcAux.h>

#define cacheOperationCode(buffer,size,call)  		\
											  	  	\
    /* Disable Interrupts */				  		\
    Uint32 intStatus = _disable_interrupts();		\
    										  	  	\
    /*  Cleanup the prefetch buffer also.*/   		\
    CSL_XMC_invalidatePrefetchBuffer();       		\
                                              	  	\
    /* Invalidate the cache. */               		\
    call(buffer, size, CACHE_FENCE_WAIT);     		\
    asm (" MFENCE ");                               \
    asm (" NOP  4");                          		\
    asm (" NOP  4");                          		\
    asm (" NOP  4");                          		\
    asm (" NOP  4");                          		\
                                              	  	\
    /* Reenable Interrupts.*/                 		\
    _restore_interrupts(intStatus);                 \

void cache_wbInvL1D(void* buffer,Uint32 size){
	cacheOperationCode(buffer,size,CACHE_wbInvL1d);
}

void cache_wbL1D(void* buffer, Uint32 size){
	cacheOperationCode(buffer,size,CACHE_wbL1d);
}

void cache_invL1D(void* buffer, Uint32 size){
	cacheOperationCode(buffer,size,CACHE_invL1d);
}

void cache_wbInvL2(void* buffer,Uint32 size){
	cacheOperationCode(buffer,size,CACHE_wbInvL2);
}

void cache_wbL2(void* buffer, Uint32 size){
	cacheOperationCode(buffer,size,CACHE_wbL2);
}

void cache_invL2(void* buffer, Uint32 size){
	cacheOperationCode(buffer,size,CACHE_invL2);
}


