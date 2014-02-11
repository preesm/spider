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

#include "DoubleLoop.h"

#define STANDALONE_APP	0

int main(int argc, char **argv) {
	UINT32 FifoIds[MAX_NB_FIFOs], FifoAddrs[MAX_NB_FIFOs];

	if(argc > 1)
		cpuId = atoi(argv[1]);
	else{
		printf("You must specify procID\n");
		return -1;
	}

	functions_tbl[0] = rdFile;
	functions_tbl[1] = broadcast;
	functions_tbl[2] = initNLoop;
	functions_tbl[3] = endNLoop;
	functions_tbl[4] = wrFile;

	//*** MLoop hierarchy ***//
	functions_tbl[5] = configM;
	functions_tbl[6] = initMLoop;
	functions_tbl[7] = f;
	functions_tbl[8] = endMLoop;

	//*** Special actors ***//
	functions_tbl[9] = RB0;
	functions_tbl[10] = RB1;
	functions_tbl[11] = M_in;
	functions_tbl[12] = line_in;
	functions_tbl[13] = line_out;

	//*** Implodes/Explodes ***//
//	functions_tbl[14] = switch_1;



	LRTInit();

#if STANDALONE_APP == 1
	printf("Standalone application..\n");

	for (i = 0; i < MAX_NB_FIFOs; i++) {
		FifoIds[i] = i;
		FifoAddrs[i] = i * IMG_SIZE;
	}
	flushFIFO(-1);	// Clear all FIFOs.

	/*** readFile ***/
	outputFifoId[0] = FifoIds[0];
	outputFifoId[1] = FifoIds[1];
	outputFifoAddr[0] = FifoAddrs[0];
	outputFifoAddr[1] = FifoAddrs[1];
	readFile(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

#else
	LRTCtrlStart();
#endif
	return 0;
}
