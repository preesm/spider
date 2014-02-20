
/****************************************************************************
 * Copyright or � or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-Fran�ois Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

//#include <string.h>
//
//#include <types.h>
//#include <hwQueues.h>
//#include <platform.h>
#include "sharedMem.h"
//
//#include <hwQueues.h>
//#include <print.h>

#include "lrt_1W1RfifoMngr.h"
#include "lrt_debug.h"


/* LOCAL VARIABLES */

void flushFIFO(UINT32 id){
	UINT32 i;
	UINT8 data = 0;
	if(id == (UINT32)-1){
		for(i=0; i<MAX_NB_FIFOs; i++)
			OS_ShMemWrite(SH_MEM_BASE_ADDR + (i * FIFO_MUTEX_SIZE), &data, FIFO_MUTEX_SIZE);
	}
	else{
		OS_ShMemWrite(SH_MEM_BASE_ADDR + (id * FIFO_MUTEX_SIZE), &data, FIFO_MUTEX_SIZE);
	}
}


void writeFifo(UINT8 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	UINT8 mutex;
	mutex = 1;
	while(mutex != 0)
		OS_ShMemRead(SH_MEM_BASE_ADDR + (id * FIFO_MUTEX_SIZE), &mutex, FIFO_MUTEX_SIZE);

	OS_ShMemWrite(addr + SH_MEM_HDR_REGION_SIZE, buffer, size);
	mutex = 1;
	OS_ShMemWrite(SH_MEM_BASE_ADDR + (id * FIFO_MUTEX_SIZE), &mutex, FIFO_MUTEX_SIZE);
}


void readFifo(UINT8 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	UINT8 mutex;
	mutex = 0;
	while(mutex != 1)
		OS_ShMemRead(SH_MEM_BASE_ADDR + (id * FIFO_MUTEX_SIZE), &mutex, FIFO_MUTEX_SIZE);

	OS_ShMemRead(addr + SH_MEM_HDR_REGION_SIZE, buffer, size);
	mutex = 0;
	OS_ShMemWrite(SH_MEM_BASE_ADDR + (id * FIFO_MUTEX_SIZE), &mutex, FIFO_MUTEX_SIZE);
}
