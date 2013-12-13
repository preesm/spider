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
#include <platform.h>
#include <lrt_core.h>
#include <lrt_taskMngr.h>

#include "definitions.h"
#include "top_AM_actions.h"

int main(int argc, char **argv) {
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
	functions_tbl[2] = inputVOL_L1;
	functions_tbl[3] = inputComplexity;
	functions_tbl[4] = inputVOLPos;
	functions_tbl[5] = inputImgDimensions;
	functions_tbl[6] = readVOP;
	functions_tbl[7] = displayVOP;
#if HIERARCHY_LEVEL > 1
	functions_tbl[8] = inputVOL_L2;
	functions_tbl[9] = inputVOPData_L2;
	functions_tbl[10] = inputFrmData_L2;
	functions_tbl[11] = inputPrevImg_L2;
	functions_tbl[12] = setVOPType;
	functions_tbl[13] = switch_0;
	functions_tbl[14] = switch_1;
	functions_tbl[15] = decodeVOP_I;
	functions_tbl[16] = decodeVOP_P;
	functions_tbl[18] = select_0;
	functions_tbl[19] = outputImg;
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
//
//	char	data_in[10];
//	char 	data_out[10];
//
//	OS_QInit();
//	printf("%i bytes read : %s\n", OS_CtrlQPop(data_in, 10), data_in);
//
//	OS_ShMemInit();
//	OS_ShMemWrite(0x10000000, data_in, 10);
//	OS_ShMemRead(0x10000000, data_out, 10);
//	printf("%s\n", data_out);


	CloseSDLDisplay();

	return 0;
}
