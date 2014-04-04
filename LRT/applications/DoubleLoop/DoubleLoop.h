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

#define	M_MAX_VALUE			3
#define	N_MAX_VALUE			3
#define MAX_DATA_SIZE		N_MAX_VALUE * M_MAX_VALUE

void rdFile(FIFO inputFIFOs[],
			FIFO outputFIFOs[],
			UINT32 params[])
{
	UINT8 N, i, j;
	UINT8 M[M_MAX_VALUE];
	UINT8 array[MAX_DATA_SIZE] = {
			1,2,3,4,5,6,7,8,9
	};

	/* initialize random seed: */

	N = 0;
	memset(M, 0, M_MAX_VALUE * sizeof(UINT8));

	//printf("Reading file:\n");
	for (i = 0; i < MAX_DATA_SIZE; i++) {
		array[i] = i;
	}

	for (i = 0; i < N_MAX_VALUE; i++) {
		// Printing the values.
		//printf("%d, %d, %d\n", array[0 + i * M_MAX_VALUE], array[1 + i * M_MAX_VALUE], array[2 + i * M_MAX_VALUE]);
		// Computing N (number of rows with a non zero at least)
		if(array[0 + i * M_MAX_VALUE] + array[1 + i * M_MAX_VALUE] + array[2 + i * M_MAX_VALUE]) N++;
		// Computing M[i] (number of non zero for each row)
		for (j = 0; j < M_MAX_VALUE; j++) {
			if(array[i + j * M_MAX_VALUE] > 0) M[i]++;
		}
	}

	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, N);
	platform_queue_push_finalize(PlatformCtrlQueue);

	platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, M_MAX_VALUE * sizeof(UINT8), (UINT8*)M);
	platform_writeFifo(outputFIFOs[1].id, outputFIFOs[1].add, MAX_DATA_SIZE * sizeof(UINT8), (UINT8*)array);
}


void initNLoop(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT8 N, i;
	UINT8 M_in[M_MAX_VALUE];
	UINT8 array_in[MAX_DATA_SIZE];
//	UINT8 M_out[M_MAX_VALUE];
//	UINT8 array_out[MAX_DATA_SIZE];

	N = params[0];
	platform_readFifo(inputFIFOs[1].id,inputFIFOs[1].add, MAX_DATA_SIZE * sizeof(UINT8), (UINT8*)array_in);
	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, M_MAX_VALUE * sizeof(UINT8), (UINT8*)M_in);

	//printf("Init N loop with N=%d array = %d", N, array_in[0]);
	for (i = 1; i < N * M_MAX_VALUE; i++) {
		//printf(", %d", array_in[i]);
	}
	//printf("\n");

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, N * sizeof(UINT8), (UINT8*)&M_in);
	platform_writeFifo(outputFIFOs[1].id,outputFIFOs[1].add, N * M_MAX_VALUE * sizeof(UINT8), (UINT8*)array_in);
}


void endNLoop(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT8 N;
	UINT8 array_in[MAX_DATA_SIZE];
//	UINT8 array_out[MAX_DATA_SIZE];

	memset(array_in, 0, MAX_DATA_SIZE * sizeof(UINT8));
	N = params[0];
	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, N * M_MAX_VALUE * sizeof(UINT8), (UINT8*)array_in);

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, MAX_DATA_SIZE * sizeof(UINT8), (UINT8*)array_in);
}


void wrFile(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT8 i;
	UINT8 array[MAX_DATA_SIZE];
	memset(array, 0, MAX_DATA_SIZE);

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, MAX_DATA_SIZE * sizeof(UINT8), (UINT8*)array);

	//printf("Writing into file:\n");
	for (i = 0; i < M_MAX_VALUE; i++) {
		//printf("%d, %d, %d\n", array[0 + i * N_MAX_VALUE], array[1 + i * N_MAX_VALUE], array[2 + i * N_MAX_VALUE]);
	}
}



/********** MLoop hierarchy *********/

void configM(FIFO inputFIFOs[],
			FIFO outputFIFOs[],
			UINT32 params[])
{
	UINT8 M;

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, sizeof(UINT8), (UINT8*)&M);
	//printf("Configure M=%d\n", M);
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
	UINT8 M, i;
	UINT8 line[M_MAX_VALUE];

	M = params[0];
	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, M_MAX_VALUE * sizeof(UINT8), (UINT8*)line);

	//printf("Init M loop with M=%d. Line out : %d", M, line[0]);

	for (i = 1; i < M; i++) {
		//printf(", %d", line[i]);
	}
	//printf("\n");

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, M * sizeof(UINT8), (UINT8*)line);
}


void f(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT8 inData;
	UINT8 outData;

	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, sizeof(UINT8), (UINT8*)&inData);

	outData = ++inData;

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, sizeof(UINT8), (UINT8*)&outData);
}


void endMLoop(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT8 M;
	UINT8 line_in[M_MAX_VALUE];

	memset(line_in, 0, M_MAX_VALUE * sizeof(UINT8));
	M = params[0];
	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, M * sizeof(UINT8), (UINT8*)line_in);

	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, M_MAX_VALUE * sizeof(UINT8), (UINT8*)line_in);
}


//******** Special actors ***********//
void RB(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbTknIn, nbTknOut, i;
	UINT32 quotient, residual;
	UINT8 data[MAX_DATA_SIZE];


	nbTknIn = params[0];
	nbTknOut = params[1];

	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), (UINT8*)data);

	//printf("Round buffering %d to %d: %d", nbTknIn, nbTknOut, data[0]);
	for (i = 1; i < nbTknIn; i++) {
		//printf(", %d", data[i]);
	}
	//printf("\n");

	if(nbTknIn == nbTknOut){
		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, nbTknIn * sizeof(UINT8), (UINT8*)data);
	}
	else if(nbTknIn < nbTknOut){
		quotient = nbTknOut / nbTknIn;
		residual = nbTknOut % nbTknIn;
		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, quotient * sizeof(UINT8), (UINT8*)data);
		if(residual > 0)
			platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, residual * sizeof(UINT8), (UINT8*)data);
	}
	else{
		//printf("Error in RB, incoming tokens > outgoing tokens\n");
		exit(-1);
	}
}

void broadcast(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbFifoOut, nbTknIn, i;
	UINT8 data[MAX_DATA_SIZE];

	nbFifoOut = params[0];
	nbTknIn = params[1];

	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), (UINT8*)data);

	//printf("Broadcasting: %d", data[0]);
	for (i = 1; i < nbTknIn; i++) {
		//printf(", %d", data[i]);
	}
	//printf("\n");

	for (i = 0; i < nbFifoOut; i++) {
		platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknIn * sizeof(UINT8), (UINT8*)data);
	}
}


void Xplode(FIFO inputFIFOs[],
		FIFO outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbFifoIn, nbFifoOut, i, index;
	UINT8 data[MAX_DATA_SIZE];
	UINT8 nbTknIn;
	UINT8 nbTknOut;


	nbFifoIn = params[0];
	nbFifoOut = params[1];
	index = 0;
	if(nbFifoIn == 1){
		/* Explode */
		nbTknIn = params[2];
		platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), (UINT8*)data);

		//printf("Exploding : %d", data[0]);
		for (i = 1; i < nbTknIn; i++) {
			//printf(", %d", data[i]);
		}
		//printf(" -> ");

		for(i=0; i<nbFifoOut; i++){
			nbTknOut = params[i + 3];
			//printf(" {%d tkn}", nbTknOut);
			platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknOut * sizeof(UINT8), (UINT8*)(&data[index]));
			index += nbTknOut;
		}
		//printf("\n");
	}else if(nbFifoOut == 1){
		/* Implode */
		//printf("Imploding : ");
		for(i=0; i<nbFifoIn; i++){
			nbTknIn = params[i + 2];
			//printf("{%d tkn}", nbTknIn);
			platform_readFifo(inputFIFOs[i].id, inputFIFOs[i].add, nbTknIn * sizeof(UINT8), (UINT8*)(&data[index]));
			index += nbTknIn;
		}
		nbTknOut = params[nbFifoIn + 2];
		//printf(" -> %d", data[0]);
		for(i=1; i<nbTknOut; i++){
			//printf(", %d", data[i]);

		}
		//printf("\n");
		platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, nbTknOut * sizeof(UINT8), (UINT8*)data);
	}else{
		//printf("Error in Xplode\n");
		exit(-1);
	}
}
