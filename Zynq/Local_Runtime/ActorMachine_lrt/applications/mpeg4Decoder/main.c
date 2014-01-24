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
#define FIFO_SIZE			5120

int main(int argc, char **argv) {
	OS_TCB *new_tcb;
	UINT32 i, nbFrames;
	UINT32 FifoIds[MAX_NB_FIFOs], FifoAddrs[MAX_NB_FIFOs];

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

	for (i = 0; i < MAX_NB_FIFOs; i++) {
		FifoIds[i] = i;
		FifoAddrs[i] = i * FIFO_SIZE;
	}
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
	inputFifoId[0] = FifoIds[4];
	inputFifoAddr[0] = FifoAddrs[4];
	outputFifoId[0] = FifoIds[0];
	outputFifoId[1] = FifoIds[1];
	outputFifoId[2] = FifoIds[2];
	outputFifoId[3] = FifoIds[3];
	outputFifoId[4] = FifoIds[4];
	outputFifoAddr[0] = FifoAddrs[0];
	outputFifoAddr[1] = FifoAddrs[1];
	outputFifoAddr[2] = FifoAddrs[2];
	outputFifoAddr[3] = FifoAddrs[3];
	outputFifoAddr[4] = FifoAddrs[4];
	readVOL(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

	while(nbFrames < 100){
	//*** broadVOL ***//
	inputFifoId[0] = FifoIds[0];
	inputFifoAddr[0] = FifoAddrs[0];
	outputFifoId[0] = FifoIds[5];
	outputFifoId[1] = FifoIds[6];
	outputFifoAddr[0] = FifoAddrs[5];
	outputFifoAddr[1] = FifoAddrs[6];
	broadVOL(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);


	//*** readVOP ***//
	inputFifoId[0] = FifoIds[6];
	inputFifoId[1] = FifoIds[1];
	inputFifoId[2] = FifoIds[2];
	inputFifoId[3] = FifoIds[9];
	inputFifoAddr[0] = FifoAddrs[6];
	inputFifoAddr[1] = FifoAddrs[1];
	inputFifoAddr[2] = FifoAddrs[2];
	inputFifoAddr[3] = FifoAddrs[9];
	outputFifoId[0] = FifoIds[7];
	outputFifoId[1] = FifoIds[8];
	outputFifoId[2] = FifoIds[9];
	outputFifoAddr[0] = FifoAddrs[7];
	outputFifoAddr[1] = FifoAddrs[8];
	outputFifoAddr[2] = FifoAddrs[9];
	readVOP(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);


	//*** decodeVOP ***//
//	inputFifoId[0] = FifoIds[5];
//	inputFifoId[1] = FifoIds[7];
//	inputFifoId[2] = FifoIds[8];
//	inputFifoId[3] = FifoIds[11];
//	inputFifoAddr[0] = FifoAddrs[5];
//	inputFifoAddr[1] = FifoAddrs[7];
//	inputFifoAddr[2] = FifoAddrs[8];
//	inputFifoAddr[3] = FifoAddrs[11];
//	outputFifoId[0] = FifoIds[10];
//	outputFifoId[1] = FifoIds[12];
//	outputFifoAddr[0] = FifoAddrs[10];
//	outputFifoAddr[1] = FifoAddrs[12];
//	decodeVOP(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

	//*** decodeVOP_I ***//
	inputFifoId[0] = FifoIds[5];
	inputFifoId[1] = FifoIds[7];
	inputFifoId[2] = FifoIds[8];
	inputFifoAddr[0] = FifoAddrs[5];
	inputFifoAddr[1] = FifoAddrs[7];
	inputFifoAddr[2] = FifoAddrs[8];
	outputFifoId[0] = FifoIds[10];
	outputFifoAddr[0] = FifoAddrs[10];
	decodeVOP_I(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

	//*** display ***//
	inputFifoId[0] = FifoIds[10];
	inputFifoId[1] = FifoIds[3];
	inputFifoAddr[0] = FifoAddrs[10];
	inputFifoAddr[1] = FifoAddrs[3];
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
