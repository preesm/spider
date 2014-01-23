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

#include "definitions.h"
#include "top_AM_actions.h"

#define STANDALONE_APP		1

int main(int argc, char **argv) {
	OS_TCB *new_tcb;
	UINT32 i, nbFrames;

	UINT32 inputFifoId[MAX_NB_FIFOs];
	UINT32 inputFifoAddr[MAX_NB_FIFOs];
	UINT32 outputFifoId[MAX_NB_FIFOs];
	UINT32 outputFifoAddr[MAX_NB_FIFOs];

	nbFrames = 0;

	if(argc > 1)
		cpuId = atoi(argv[1]);
	else{
		printf("You must specify procID\n");
		return -1;
	}

	// Top L0.
	functions_tbl[0] = readVOL;
//	functions_tbl[1] = decodeDsply;
	// Level L1.
//	functions_tbl[2] = inputVOL_L1;
//	functions_tbl[3] = inputComplexity;
//	functions_tbl[4] = inputVOLPos;
//	functions_tbl[5] = broadVOL;
	functions_tbl[6] = readVOP;
	functions_tbl[7] = displayVOP;
#if HIERARCHY_LEVEL > 1
//	functions_tbl[8] = inputVOL_L2;
//	functions_tbl[9] = inputVOPData_L2;
//	functions_tbl[10] = inputFrmData_L2;
//	functions_tbl[11] = inputPrevImg_L2;
	functions_tbl[12] = setVOPType;
	functions_tbl[13] = switch_0;
	functions_tbl[14] = switch_1;
	functions_tbl[15] = decodeVOP_I;
	functions_tbl[16] = decodeVOP_P;
	functions_tbl[18] = select_0;
//	functions_tbl[19] = outputImg;
#else
	functions_tbl[8] = decodeVOP;
#endif

//	UINT32 data = 1;
//	FILE* pFile = fopen(SH_MEM_FILE_PATH, "rb+");
//	fseek(pFile, 0, SEEK_SET);
//	fwrite(&data, 8, 1, pFile);
//	fclose(pFile);

	Init_SDL(16, 720, 576);


	LRTInit();

#if STANDALONE_APP == 1
	printf("Standalone application..\n");

	flushFIFO(-1);	// Clear all FIFOs.


	/*
	 * Creating tasks.
	 */
	//*** readVOL ***//
//	new_tcb = LrtTaskCreate();
//	new_tcb->functionId = 0;
//	new_tcb->isAM = FALSE;
//
//	new_tcb->actor = &LRTActorTbl[new_tcb->OSTCBId];
//
//	new_tcb->actor->nbInputFifos = 1;
//	new_tcb->actor->inputFifoId[0] = 0;
//	new_tcb->actor->inputFifoDataOff[0] = 0;
//
//	new_tcb->actor->nbOutputFifos = 5;
//	new_tcb->actor->outputFifoId[0] = 1;
//	new_tcb->actor->outputFifoId[1] = 2;
//	new_tcb->actor->outputFifoId[2] = 3;
//	new_tcb->actor->outputFifoId[3] = 4;
//	new_tcb->actor->outputFifoId[4] = 0;
//	new_tcb->actor->outputFifoDataOff[0] = 1024;
//	new_tcb->actor->outputFifoDataOff[1] = 2048;
//	new_tcb->actor->outputFifoDataOff[2] = 3072;
//	new_tcb->actor->outputFifoDataOff[3] = 4096;
//	new_tcb->actor->outputFifoDataOff[4] = 0;
//
//	new_tcb->task_func = functions_tbl[new_tcb->functionId];
//	new_tcb->stop = TRUE;
	inputFifoId[0] = 4;
	inputFifoAddr[0] = 4096;
	outputFifoId[0] = 0;
	outputFifoId[1] = 1;
	outputFifoId[2] = 2;
	outputFifoId[3] = 3;
	outputFifoId[4] = 4;
	outputFifoAddr[0] = 0;
	outputFifoAddr[1] = 1024;
	outputFifoAddr[2] = 2048;
	outputFifoAddr[3] = 3072;
	outputFifoAddr[4] = 4096;
	readVOL(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

	while(nbFrames < 100){
	//*** broadVOL ***//
	inputFifoId[0] = 0;
	inputFifoAddr[0] = 0;
	outputFifoId[0] = 5;
	outputFifoId[1] = 6;
	outputFifoAddr[0] = 5120;
	outputFifoAddr[1] = 6144;
	broadVOL(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);


	//*** readVOP ***//
	inputFifoId[0] = 6;
	inputFifoId[1] = 1;
	inputFifoId[2] = 2;
	inputFifoId[3] = 9;
	inputFifoAddr[0] = 6144;
	inputFifoAddr[1] = 1024;
	inputFifoAddr[2] = 2048;
	inputFifoAddr[3] = 9216;
	outputFifoId[0] = 7;
	outputFifoId[1] = 8;
	outputFifoId[2] = 9;
	outputFifoAddr[0] = 7168;
	outputFifoAddr[1] = 8192;
	outputFifoAddr[2] = 9216;
	readVOP(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);


//	//*** decodeVOP ***//
	inputFifoId[0] = 5;
	inputFifoId[1] = 7;
	inputFifoId[2] = 8;
	inputFifoId[3] = 11;
	inputFifoAddr[0] = 5120;
	inputFifoAddr[1] = 7168;
	inputFifoAddr[2] = 8192;
	inputFifoAddr[3] = 11264;
	outputFifoId[0] = 10;
	outputFifoId[1] = 12;
	outputFifoAddr[0] = 10240;
	outputFifoAddr[1] = 12088;
	decodeVOP(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

//	//*** decodeVOP_I ***//
//	inputFifoId[0] = 5;
//	inputFifoId[1] = 7;
//	inputFifoId[2] = 8;
//	inputFifoAddr[0] = 5120;
//	inputFifoAddr[1] = 7168;
//	inputFifoAddr[2] = 8192;
//	outputFifoId[0] = 10;
//	outputFifoAddr[0] = 10240;
//	decodeVOP_I(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

	//*** display ***//
	inputFifoId[0] = 10;
	inputFifoId[1] = 3;
	inputFifoAddr[0] = 10240;
	inputFifoAddr[1] = 3072;
	displayVOP(inputFifoId, inputFifoAddr, 0, 0, 0);

	nbFrames++;
	}
//	LRTStart();

#else
	LRTInitCtrl();
	LRTCtrlStart();
#endif

	CloseSDLDisplay();

	return 0;
}
