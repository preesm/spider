/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sharedMem.h>
#include <hwQueues.h>
#include <lrt_1W1RfifoMngr.h>
#include <platform.h>
#include <lrt_core.h>
#include <lrt_taskMngr.h>

//#include "definitions.h"
//#include "top_AM_actions.h"

#define STANDALONE_APP		1



void readVOL(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT8 inData;
	readFifo(inputFIFOIds[0],inputFIFOAddrs[0], sizeof(inData), &inData);

	/* Sending data */
	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(inData), &inData);
	inData++;
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], sizeof(inData), &inData);
	inData++;
	writeFifo(outputFIFOIds[2], outputFIFOAddrs[2], sizeof(inData), &inData);
	inData++;
	writeFifo(outputFIFOIds[3], outputFIFOAddrs[3], sizeof(inData), &inData);
	inData++;
	writeFifo(outputFIFOIds[4], outputFIFOAddrs[4], sizeof(inData), &inData);
	inData++;
}


void decodeDsply(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[])
{

	UINT8 inData;
	readFifo(inputFIFOIds[0],inputFIFOAddrs[0], sizeof(inData), &inData);
	printf("%d\n", inData);
	readFifo(inputFIFOIds[1],inputFIFOAddrs[1], sizeof(inData), &inData);
	printf("%d\n", inData);
	readFifo(inputFIFOIds[2],inputFIFOAddrs[2], sizeof(inData), &inData);
	printf("%d\n", inData);
	readFifo(inputFIFOIds[3],inputFIFOAddrs[3], sizeof(inData), &inData);
	printf("%d\n", inData);
}



int main(int argc, char **argv) {
	OS_TCB *new_tcb;
	UINT32 i;

//	if(argc > 1)
//		cpuId = atoi(argv[1]);
//	else{
//		printf("You must specify procID\n");
//		return -1;
//	}
	cpuId = 0;

	// Top L0.
	functions_tbl[0] = readVOL;
	functions_tbl[1] = decodeDsply;


	LRTInit();

#if STANDALONE_APP == 1
	printf("Standalone application..\n");

	flushFIFO(-1);	// Clear all FIFOs.

	/*
	 * Creating tasks.
	 */
	// readVOL
	new_tcb = LrtTaskCreate();
	new_tcb->functionId = 0;
	new_tcb->isAM = FALSE;

	new_tcb->actor = &LRTActorTbl[new_tcb->OSTCBId];

	new_tcb->actor->nbInputFifos = 1;
	new_tcb->actor->inputFifoId[0] = 0;
	new_tcb->actor->inputFifoDataOff[0] = 0;

	new_tcb->actor->nbOutputFifos = 5;
	new_tcb->actor->outputFifoId[0] = 1;
	new_tcb->actor->outputFifoId[1] = 2;
	new_tcb->actor->outputFifoId[2] = 3;
	new_tcb->actor->outputFifoId[3] = 4;
	new_tcb->actor->outputFifoId[4] = 0;
	new_tcb->actor->outputFifoDataOff[0] = 1024;
	new_tcb->actor->outputFifoDataOff[1] = 2048;
	new_tcb->actor->outputFifoDataOff[2] = 3072;
	new_tcb->actor->outputFifoDataOff[3] = 4096;
	new_tcb->actor->outputFifoDataOff[4] = 0;

	new_tcb->task_func = functions_tbl[new_tcb->functionId];
	new_tcb->stop = TRUE;

	// decodeDsply
	new_tcb = LrtTaskCreate();
	new_tcb->functionId = 1;
	new_tcb->isAM = FALSE;

	new_tcb->actor = &LRTActorTbl[new_tcb->OSTCBId];

	new_tcb->actor->nbInputFifos = 4;
	new_tcb->actor->inputFifoId[0] = 1;
	new_tcb->actor->inputFifoId[1] = 2;
	new_tcb->actor->inputFifoId[2] = 3;
	new_tcb->actor->inputFifoId[3] = 4;
	new_tcb->actor->inputFifoDataOff[0] = 1024;
	new_tcb->actor->inputFifoDataOff[1] = 2048;
	new_tcb->actor->inputFifoDataOff[2] = 3072;
	new_tcb->actor->inputFifoDataOff[3] = 4096;

	new_tcb->actor->nbOutputFifos = 0;

	new_tcb->task_func = functions_tbl[new_tcb->functionId];
	new_tcb->stop = TRUE;


	LRTStart();

#else
	LRTCtrlStart();
#endif


	return 0;
}
