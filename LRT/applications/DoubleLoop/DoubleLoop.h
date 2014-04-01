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

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string.h>

#define	M_MAX_VALUE			3
#define	N_MAX_VALUE			3
#define MAX_DATA_SIZE		N_MAX_VALUE * M_MAX_VALUE

void rdFile(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 N, i, j;
	UINT16 M[M_MAX_VALUE];
	UINT16 array[MAX_DATA_SIZE] = {
			1,2,3,4,5,6,7,8,9
	};
	UINT32 vxId;

	/* initialize random seed: */
	srand (time(NULL));

	N = 0;
	memset(M, 0, M_MAX_VALUE * sizeof(UINT16));

	//printf("Reading file:\n");
	for (i = 0; i < MAX_DATA_SIZE; i++) {
		array[i] = (rand() % 10) + 1;
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
	platform_queue_push_UINT32(PlatformCtrlQueue, rtGetVxId());
	platform_queue_push_UINT32(PlatformCtrlQueue, N);

	platform_writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)M);
	platform_writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], MAX_DATA_SIZE * sizeof(UINT16), (UINT8*)array);
}


void initNLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 N, i;
	UINT16 M_in[M_MAX_VALUE];
	UINT16 array_in[MAX_DATA_SIZE];
//	UINT16 M_out[M_MAX_VALUE];
//	UINT16 array_out[MAX_DATA_SIZE];

	N = params[0];
	platform_readFifo(inputFIFOIds[1],inputFIFOAddrs[1], MAX_DATA_SIZE * sizeof(UINT16), (UINT8*)array_in);
	platform_readFifo(inputFIFOIds[0],inputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)M_in);

	//printf("Init N loop with N=%d array = %d", N, array_in[0]);
	for (i = 1; i < N * M_MAX_VALUE; i++) {
		//printf(", %d", array_in[i]);
	}
	//printf("\n");

	platform_writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], N * sizeof(UINT16), (UINT8*)&M_in);
	platform_writeFifo(outputFIFOIds[1],outputFIFOAddrs[1], N * M_MAX_VALUE * sizeof(UINT16), (UINT8*)array_in);
}


void endNLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 N;
	UINT16 array_in[MAX_DATA_SIZE];
//	UINT16 array_out[MAX_DATA_SIZE];

	memset(array_in, 0, MAX_DATA_SIZE * sizeof(UINT16));
	N = params[0];
	platform_readFifo(inputFIFOIds[0],inputFIFOAddrs[0], N * M_MAX_VALUE * sizeof(UINT16), (UINT8*)array_in);

	platform_writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], MAX_DATA_SIZE * sizeof(UINT16), (UINT8*)array_in);
}


void wrFile(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M[M_MAX_VALUE], i;
	UINT16 array[MAX_DATA_SIZE];
	memset(array, 0, MAX_DATA_SIZE);

	platform_readFifo(inputFIFOIds[0], inputFIFOAddrs[0], MAX_DATA_SIZE * sizeof(UINT16), (UINT8*)array);

	//printf("Writing into file:\n");
	for (i = 0; i < M_MAX_VALUE; i++) {
		//printf("%d, %d, %d\n", array[0 + i * N_MAX_VALUE], array[1 + i * N_MAX_VALUE], array[2 + i * N_MAX_VALUE]);
	}
}



/********** MLoop hierarchy *********/

void configM(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M;

	platform_readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&M);
	//printf("Configure M=%d\n", M);
	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, rtGetVxId());
	platform_queue_push_UINT32(PlatformCtrlQueue, M);
}


void initMLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M, i;
	UINT16 line[M_MAX_VALUE];

	M = params[0];
	platform_readFifo(inputFIFOIds[0], inputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)line);

	//printf("Init M loop with M=%d. Line out : %d", M, line[0]);

	for (i = 1; i < M; i++) {
		//printf(", %d", line[i]);
	}
	//printf("\n");

	platform_writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], M * sizeof(UINT16), (UINT8*)line);
}


void f(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 inData;
	UINT16 outData;

	platform_readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&inData);

	outData = ++inData;

	platform_writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&outData);
}


void endMLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M;
	UINT16 line_in[M_MAX_VALUE];

	memset(line_in, 0, M_MAX_VALUE * sizeof(UINT16));
	M = params[0];
	platform_readFifo(inputFIFOIds[0], inputFIFOAddrs[0], M * sizeof(UINT16), (UINT8*)line_in);

	platform_writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)line_in);
}


//******** Special actors ***********//
void RB(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT32 nbTknIn, nbTknOut, i;
	UINT32 quotient, residual;
	UINT16 data[MAX_DATA_SIZE];


	nbTknIn = params[0];
	nbTknOut = params[1];

	platform_readFifo(inputFIFOIds[0],inputFIFOAddrs[0], nbTknIn * sizeof(UINT16), (UINT8*)data);

	//printf("Round buffering %d to %d: %d", nbTknIn, nbTknOut, data[0]);
	for (i = 1; i < nbTknIn; i++) {
		//printf(", %d", data[i]);
	}
	//printf("\n");

	if(nbTknIn == nbTknOut){
		platform_writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], nbTknIn * sizeof(UINT16), (UINT8*)data);
	}
	else if(nbTknIn < nbTknOut){
		quotient = nbTknOut / nbTknIn;
		residual = nbTknOut % nbTknIn;
		platform_writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], quotient * sizeof(UINT16), (UINT8*)data);
		if(residual > 0)
			platform_writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], residual * sizeof(UINT16), (UINT8*)data);
	}
	else{
		//printf("Error in RB, incoming tokens > outgoing tokens\n");
		exit(-1);
	}
}

void broadcast(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT32 nbFifoOut, nbTknIn, i;
	UINT16 data[MAX_DATA_SIZE];

	nbFifoOut = params[0];
	nbTknIn = params[1];

	platform_readFifo(inputFIFOIds[0],inputFIFOAddrs[0], nbTknIn * sizeof(UINT16), (UINT8*)data);

	//printf("Broadcasting: %d", data[0]);
	for (i = 1; i < nbTknIn; i++) {
		//printf(", %d", data[i]);
	}
	//printf("\n");

	for (i = 0; i < nbFifoOut; i++) {
		platform_writeFifo(outputFIFOIds[i], outputFIFOAddrs[i], nbTknIn * sizeof(UINT16), (UINT8*)data);
	}
}


void Xplode(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT32 nbFifoIn, nbFifoOut, i, j, index;
	UINT16 data[MAX_DATA_SIZE];
	UINT16 nbTknIn;
	UINT16 nbTknOut;


	nbFifoIn = params[0];
	nbFifoOut = params[1];
	index = 0;
	if(nbFifoIn == 1){
		/* Explode */
		nbTknIn = params[2];
		platform_readFifo(inputFIFOIds[0],inputFIFOAddrs[0], nbTknIn * sizeof(UINT16), (UINT8*)data);

		//printf("Exploding : %d", data[0]);
		for (i = 1; i < nbTknIn; i++) {
			//printf(", %d", data[i]);
		}
		//printf(" -> ");

		for(i=0; i<nbFifoOut; i++){
			nbTknOut = params[i + 3];
			//printf(" {%d tkn}", nbTknOut);
			platform_writeFifo(outputFIFOIds[i], outputFIFOAddrs[i], nbTknOut * sizeof(UINT16), (UINT8*)(&data[index]));
			index += nbTknOut;
		}
		//printf("\n");
	}else if(nbFifoOut == 1){
		/* Implode */
		//printf("Imploding : ");
		for(i=0; i<nbFifoIn; i++){
			nbTknIn = params[i + 2];
			//printf("{%d tkn}", nbTknIn);
			platform_readFifo(inputFIFOIds[i], inputFIFOAddrs[i], nbTknIn * sizeof(UINT16), (UINT8*)(&data[index]));
			index += nbTknIn;
		}
		nbTknOut = params[nbFifoIn + 2];
		//printf(" -> %d", data[0]);
		for(i=1; i<nbTknOut; i++){
			//printf(", %d", data[i]);

		}
		//printf("\n");
		platform_writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], nbTknOut * sizeof(UINT16), (UINT8*)data);
	}else{
		//printf("Error in Xplode\n");
		exit(-1);
	}
}
