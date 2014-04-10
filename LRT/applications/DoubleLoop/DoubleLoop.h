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

void rdFile(FIFO inputFIFOs[],
			FIFO outputFIFOs[],
			UINT32 params[])
{
	UINT8 N, i;//, j;
	UINT32 NMAX,MMAX;
	UINT8* array, *M;

	/* initialize random seed: */

//	N = 0;
//	memset(M, 0, M_MAX_VALUE * sizeof(UINT8));
//
//	//printf("Reading file:\n");
//	for (i = 0; i < MAX_DATA_SIZE; i++) {
//		array[i] = i;
//	}
//
//	for (i = 0; i < N_MAX_VALUE; i++) {
//		// Printing the values.
//		//printf("%d, %d, %d\n", array[0 + i * M_MAX_VALUE], array[1 + i * M_MAX_VALUE], array[2 + i * M_MAX_VALUE]);
//		// Computing N (number of rows with a non zero at least)
//		if(array[0 + i * M_MAX_VALUE] + array[1 + i * M_MAX_VALUE] + array[2 + i * M_MAX_VALUE]) N++;
//		// Computing M[i] (number of non zero for each row)
//		for (j = 0; j < M_MAX_VALUE; j++) {
//			if(array[i + j * M_MAX_VALUE] > 0) M[i]++;
//		}
//	}
	NMAX = params[0];
	MMAX = params[1];

	array = OSAllocWorkingMemory(NMAX*MMAX);
	M = OSAllocWorkingMemory(NMAX);

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

	platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, MMAX * sizeof(UINT8), M);
	platform_writeFifo(outputFIFOs[1].id, outputFIFOs[1].add, NMAX*MMAX * sizeof(UINT8), array);
}


void initNLoop(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
//	UINT32 i;
	UINT32 N,NMAX,MMAX;
	UINT8 *M, *array;

	N = params[0];
	NMAX = params[1];
	MMAX = params[2];

	array = OSAllocWorkingMemory(NMAX*MMAX);
	M = OSAllocWorkingMemory(NMAX);

	platform_readFifo(inputFIFOs[1].id,inputFIFOs[1].add, NMAX*MMAX * sizeof(UINT8), array);
	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, NMAX * sizeof(UINT8), M);

//	printf("Init N loop with N=%d M = %d", N, M[0]);
//	for (i = 1; i < N; i++) {
//		printf(", %d", M[i]);
//	}
//	printf("\n");

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, N * sizeof(UINT8), M);
	platform_writeFifo(outputFIFOs[1].id,outputFIFOs[1].add, N * MMAX * sizeof(UINT8), array);
}


void endNLoop(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 N,NMAX,MMAX;
	UINT8 *array;

	N = params[0];
	NMAX = params[1];
	MMAX = params[2];

	array = OSAllocWorkingMemory(NMAX*MMAX);

	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, N * MMAX * sizeof(UINT8), array);
	memset(array+N*MMAX,0,(NMAX-N)*MMAX);

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, MMAX*NMAX * sizeof(UINT8), array);
}


void wrFile(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 NMAX,MMAX;
	UINT8 *array;

	NMAX = params[0];
	MMAX = params[1];

	array = OSAllocWorkingMemory(NMAX*MMAX);

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, NMAX*MMAX * sizeof(UINT8), array);

//	//printf("Writing into file:\n");
//	for (i = 0; i < M_MAX_VALUE; i++) {
//		//printf("%d, %d, %d\n", array[0 + i * N_MAX_VALUE], array[1 + i * N_MAX_VALUE], array[2 + i * N_MAX_VALUE]);
//	}
	int i;
	for(i=0; i<NMAX*MMAX; ++i){
		if(array[i] != (unsigned char)(NMAX*MMAX+i+1)){
			printf("Nb(%d): Error bad value\n", NMAX*MMAX);
			printf("(%d)(%d,%d) expected (%d) get (%d)\n", i, i/NMAX*MMAX, i%NMAX*MMAX, NMAX*MMAX+i+1, array[i]);
			abort();
		}
	}
}



/********** MLoop hierarchy *********/

void configM(FIFO inputFIFOs[],
			FIFO outputFIFOs[],
			UINT32 params[])
{
	UINT8 M;

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, sizeof(UINT8), &M);
//	printf("Configure M=%d\n", M);

	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, M);
	platform_queue_push_finalize(PlatformCtrlQueue);
}


void initMLoop(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 M, MMAX;
	UINT8 *line;

	M = params[0];
	MMAX = params[1];

	line = OSAllocWorkingMemory(MMAX);

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, MMAX * sizeof(UINT8), line);

	//printf("Init M loop with M=%d. Line out : %d", M, line[0]);

//	for (i = 1; i < M; i++) {
//		printf(", %d", line[i]);
//	}
//	printf("\n");

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, M * sizeof(UINT8), line);
}


void f(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT8 inData;
	UINT8 outData;

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, sizeof(UINT8), &inData);

	outData = ++inData;

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, sizeof(UINT8), &outData);
//	printf("F -> %d\n", outputFIFOs[0].id);
}


void endMLoop(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 M, MMAX;
	UINT8 *line;

	M = params[0];
	MMAX = params[1];

	line = OSAllocWorkingMemory(MMAX);

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, M * sizeof(UINT8), line);
	memset(line+M,0,MMAX-M);

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, MMAX * sizeof(UINT8), line);
}


//******** Special actors ***********//
void RB(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbTknIn, nbTknOut;//, i;
	UINT32 quotient, residual;
	UINT8 *data;


	nbTknIn = params[0];
	nbTknOut = params[1];

	data = OSAllocWorkingMemory(MAX(nbTknIn,nbTknOut));

	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);


//	printf("Round buffering %d to %d: %d", nbTknIn, nbTknOut, data[0]);
//	for (i = 1; i < nbTknIn; i++) {
//		printf(", %d", data[i]);
//	}
//	printf("\n");

	if(nbTknIn == nbTknOut){
		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);
	}
	else if(nbTknIn < nbTknOut){
		quotient = nbTknOut / nbTknIn;
		residual = nbTknOut % nbTknIn;
		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, quotient * sizeof(UINT8), data);
		if(residual > 0)
			platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, residual * sizeof(UINT8), data);
	}
	else{
		printf("Error in RB, incoming tokens > outgoing tokens\n");
		exit(-1);
	}
}

void broadcast(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbFifoOut, nbTknIn, i;
	UINT8 *data;

	nbFifoOut = params[0];
	nbTknIn = params[1];

	data = OSAllocWorkingMemory(nbTknIn);

	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);

//	printf("Broadcasting: %d", data[0]);
//	for (i = 1; i < nbTknIn; i++) {
//		printf(", %d", data[i]);
//	}
//	printf("\n");

	for (i = 0; i < nbFifoOut; i++) {
		platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknIn * sizeof(UINT8), data);
	}
}


void Xplode(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbFifoIn, nbFifoOut, i, index;
	UINT8 *data;
	UINT32 nbTknIn, nbTknOut;


	nbFifoIn = params[0];
	nbFifoOut = params[1];
	index = 0;
	if(nbFifoIn == 1){
		/* Explode */
		nbTknIn = params[2];
		data = OSAllocWorkingMemory(nbTknIn);

		platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);

//		printf("Exploding : %d", data[0]);
//		for (i = 1; i < nbTknIn; i++) {
//			printf(", %d", data[i]);
//		}
//		printf(" -> ");

		for(i=0; i<nbFifoOut; i++){
			nbTknOut = params[i + 3];
//			printf(" {%d tkn}", nbTknOut);
			platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknOut * sizeof(UINT8), (&data[index]));
			index += nbTknOut;
		}
//		printf("\n");
	}else if(nbFifoOut == 1){
		/* Implode */
//		printf("Imploding : ");
		nbTknOut = params[nbFifoIn + 2];
		data = OSAllocWorkingMemory(nbTknOut);

		for(i=0; i<nbFifoIn; i++){
			nbTknIn = params[i + 2];
//			printf("{%d tkn}", nbTknIn);
			platform_readFifo(inputFIFOs[i].id, inputFIFOs[i].add, nbTknIn * sizeof(UINT8), (&data[index]));
			index += nbTknIn;
		}
//		printf(" -> %d", data[0]);
//		for(i=1; i<nbTknOut; i++){
//			printf(", %d", data[i]);
//
//		}
//		printf("\n");
		platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, nbTknOut * sizeof(UINT8), data);
	}else{
		printf("Error in Xplode\n");
		exit(-1);
	}
}
