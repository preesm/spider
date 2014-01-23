/*
 * main.c
 *
 *  Created on: 13 sept. 2013
 *      Author: yoliva
 */

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
	LRTInitCtrl();
	LRTCtrlStart();
#endif


	return 0;
}
