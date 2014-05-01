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

#include <string.h>

#define MAX(a,b) (a>b?a:b)

void rdFile(UINT8* inputFIFOs[],
			UINT8* outputFIFOs[],
			UINT32 params[])
{
	UINT8 N, i;//, j;
	UINT32 NMAX,MMAX;
	UINT8* array, *M;

	NMAX = params[0];
	MMAX = params[1];

	M = outputFIFOs[0];//OSAllocWorkingMemory(NMAX);
	array = outputFIFOs[1];//OSAllocWorkingMemory(NMAX*MMAX);

	N = NMAX;
	for(i=0; i<NMAX*MMAX; ++i){
		array[i] = NMAX*MMAX+i;
	}

	for(i=0; i<NMAX; ++i){
		M[i] = MMAX;
	}

	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, N);
	platform_queue_push_finalize(PlatformCtrlQueue);

//	platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, MMAX * sizeof(UINT8), M);
//	platform_writeFifo(outputFIFOs[1].id, outputFIFOs[1].add, NMAX*MMAX * sizeof(UINT8), array);
}


void initNLoop(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
//	UINT32 i;
	UINT32 N,NMAX,MMAX;
	UINT8 *M_in, *array_in;
	UINT8 *M_out, *array_out;

	N = params[0];
	NMAX = params[1];
	MMAX = params[2];

	M_in = inputFIFOs[0];//OSAllocWorkingMemory(NMAX);
	array_in = inputFIFOs[1];//OSAllocWorkingMemory(NMAX*MMAX);

	M_out = outputFIFOs[0];
	array_out = outputFIFOs[1];

	memcpy(M_out, M_in, N);
	memcpy(array_out, array_in, N*MMAX);
//	platform_readFifo(inputFIFOs[1].id,inputFIFOs[1].add, NMAX*MMAX * sizeof(UINT8), array);
//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, NMAX * sizeof(UINT8), M);

//	printf("Init N loop with N=%d M = %d", N, M[0]);
//	for (i = 1; i < N; i++) {
//		printf(", %d", M[i]);
//	}
//	printf("\n");

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, N * sizeof(UINT8), M);
//	platform_writeFifo(outputFIFOs[1].id,outputFIFOs[1].add, N * MMAX * sizeof(UINT8), array);
}


void endNLoop(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 N,NMAX,MMAX;
	UINT8 *array_in, *array_out;

	N = params[0];
	NMAX = params[1];
	MMAX = params[2];

	array_in = inputFIFOs[0];// OSAllocWorkingMemory(NMAX*MMAX);
	array_out = outputFIFOs[0];

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, N * MMAX * sizeof(UINT8), array);
//	memset(array+N*MMAX,0,(NMAX-N)*MMAX);array = inputFIFOs[0];

	memcpy(array_out, array_in, N*MMAX);
	memset(array_out+N*MMAX,0,(NMAX-N)*MMAX);

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, MMAX*NMAX * sizeof(UINT8), array);
}


void wrFile(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 NMAX,MMAX;
	UINT8 *array_in;

	NMAX = params[0];
	MMAX = params[1];

	array_in = inputFIFOs[0];//OSAllocWorkingMemory(NMAX*MMAX);

//	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, NMAX*MMAX * sizeof(UINT8), array);

//	//printf("Writing into file:\n");
//	for (i = 0; i < M_MAX_VALUE; i++) {
//		//printf("%d, %d, %d\n", array[0 + i * N_MAX_VALUE], array[1 + i * N_MAX_VALUE], array[2 + i * N_MAX_VALUE]);
//	}
	int i;
	for(i=0; i<NMAX*MMAX; ++i){
		if(array_in[i] != (unsigned char)(NMAX*MMAX+i+1)){
			printf("Nb(%d): Error bad value\n", NMAX*MMAX);
			printf("(%d)(%d,%d) expected (%d) get (%d)\n", i, i/NMAX*MMAX, i%NMAX*MMAX, NMAX*MMAX+i+1, array_in[i]);
			abort();
		}
	}
}



/********** MLoop hierarchy *********/

void configM(UINT8* inputFIFOs[],
			UINT8* outputFIFOs[],
			UINT32 params[])
{
	UINT8 M;

	M = *(inputFIFOs[0]);
//	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, sizeof(UINT8), &M);
//	printf("Configure M=%d\n", M);

	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, M);
	platform_queue_push_finalize(PlatformCtrlQueue);
}


void initMLoop(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 M, MMAX;
	UINT8 *line_in, *line_out;

	M = params[0];
	MMAX = params[1];

	line_in = inputFIFOs[0];//OSAllocWorkingMemory(MMAX);
	line_out = outputFIFOs[0];

	memcpy(line_out, line_in, M);

//	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, MMAX * sizeof(UINT8), line);

	//printf("Init M loop with M=%d. Line out : %d", M, line[0]);

//	for (i = 1; i < M; i++) {
//		printf(", %d", line[i]);
//	}
//	printf("\n");

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, M * sizeof(UINT8), line);
}


void f(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
//	UINT8 inData;
//	UINT8 outData;

	*(outputFIFOs[0]) = *(inputFIFOs[0])+1;

//	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, sizeof(UINT8), &inData);

//	outData = ++inData;

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, sizeof(UINT8), &outData);
//	printf("F -> %d\n", outputFIFOs[0].id);
}


void endMLoop(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 M, MMAX;
	UINT8 *line_in, *line_out;

	M = params[0];
	MMAX = params[1];

	line_in = inputFIFOs[0]; //OSAllocWorkingMemory(MMAX);
	line_out = outputFIFOs[0];

	memcpy(line_out, line_in, M);
	memset(line_out+M,0,MMAX-M);

//	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, M * sizeof(UINT8), line);
//	memset(line+M,0,MMAX-M);

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, MMAX * sizeof(UINT8), line);
}


//******** Special actors ***********//
void RB(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
//	UINT32 nbTknIn, nbTknOut;//, i;
//	UINT32 quotient, residual;
//	UINT8 *data;
//
//
//	nbTknIn = params[0];
//	nbTknOut = params[1];
//
//	data = OSAllocWorkingMemory(MAX(nbTknIn,nbTknOut));
//
//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);
//
//
////	printf("Round buffering %d to %d: %d", nbTknIn, nbTknOut, data[0]);
////	for (i = 1; i < nbTknIn; i++) {
////		printf(", %d", data[i]);
////	}
////	printf("\n");
//
//	if(nbTknIn == nbTknOut){
//		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);
//	}
//	else if(nbTknIn < nbTknOut){
//		quotient = nbTknOut / nbTknIn;
//		residual = nbTknOut % nbTknIn;
//		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, quotient * sizeof(UINT8), data);
//		if(residual > 0)
//			platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, residual * sizeof(UINT8), data);
//	}
//	else{
		printf("Error in RB, incoming tokens > outgoing tokens\n");
//		exit(-1);
//	}
}

void broadcast(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbFifoOut, nbTknIn, i;
	UINT8 *data;

	nbFifoOut = params[0];
	nbTknIn = params[1];

	data = OSAllocWorkingMemory(nbTknIn);

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);

//	printf("Broadcasting: %d", data[0]);
//	for (i = 1; i < nbTknIn; i++) {
//		printf(", %d", data[i]);
//	}
//	printf("\n");

//	for (i = 0; i < nbFifoOut; i++) {
//		platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknIn * sizeof(UINT8), data);
//	}
	printf("Error in Broadcast\n");
}


void Xplode(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
//	UINT32 nbFifoIn, nbFifoOut, i, index;
//	UINT8 *data;
//	UINT32 nbTknIn, nbTknOut;
//	UINT32 inFifoBase, outFifoBase;

//	nbFifoIn = params[0];
//	inFifoBase = params[1];
//	nbFifoOut = params[1];
//	outFifoBase = params[3];

//	index = 0;
//	if(nbFifoIn == 1){
		/* Explode */
//		nbTknIn = params[2];
//		data = OSAllocWorkingMemory(nbTknIn);

//		platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn, data);

//		printf("Exploding : {%d tkn} -> ", nbTknIn);
//		for(i=0; i<nbFifoOut; i++){
//			nbTknOut = params[i + 3];
//			printf(" {%d tkn}", nbTknOut);
//			memcpy(outputFIFOs[i], inputFIFOs[0]+index, nbTknOut);
//			platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknOut, data+index);
//			index += nbTknOut;
//		}
//		printf("\n");
//	}else if(nbFifoOut == 1){
		/* Implode */
//		printf("Imploding : ");
//		nbTknOut = params[nbFifoIn + 2];
//		data = OSAllocWorkingMemory(nbTknOut);

//		for(i=0; i<nbFifoIn; i++){
//			nbTknIn = params[i + 2];
//			printf("{%d tkn}", nbTknIn);
//			platform_readFifo(inputFIFOs[i].id, inputFIFOs[i].add, nbTknIn, data+index);
//			memcpy(outputFIFOs[0]+index, inputFIFOs[i], nbTknIn);
//			index += nbTknIn;
//		}
//		printf(" -> {%d tkn}", nbTknOut);
//		printf("\n");

//		platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, nbTknOut, data);
//	}else{
//		printf("Error in Xplode\n");
//		exit(-1);
//	}
}
