
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/
#include <string.h>

#define	M_MAX_VALUE			3
#define	N_MAX_VALUE			3

void rdFile(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 N;
	UINT16 M[M_MAX_VALUE];
	UINT16 array[M_MAX_VALUE][N_MAX_VALUE] = {{1, 1, 0}, {1, 1, 0}, {0, 0, 0}};

	N = 2;
	M[0] = 2;
	M[1] = 2;
	M[2] = 0;

	// Sending parameter's value.
	RTQueuePush_UINT32(RTInfoQueue, N);

	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)M);
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], M_MAX_VALUE * N_MAX_VALUE * sizeof(UINT16), (UINT8*)array);
}


void initNLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 N;
	UINT16 M_in[M_MAX_VALUE];
	UINT16 array_in[M_MAX_VALUE * N_MAX_VALUE];
//	UINT16 M_out[M_MAX_VALUE];
//	UINT16 array_out[M_MAX_VALUE * N_MAX_VALUE];

	N = params[0];
	readFifo(inputFIFOIds[0],inputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)M_in);
	readFifo(inputFIFOIds[1],inputFIFOAddrs[1], M_MAX_VALUE * N_MAX_VALUE * sizeof(UINT16), (UINT8*)array_in);

	writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], N * sizeof(UINT16), (UINT8*)&M_in);
	writeFifo(outputFIFOIds[1],outputFIFOAddrs[1], N * M_MAX_VALUE * sizeof(UINT16), (UINT8*)array_in);
}


void endNLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 N;
	UINT16 array_in[M_MAX_VALUE * N_MAX_VALUE];
//	UINT16 array_out[M_MAX_VALUE * N_MAX_VALUE];

	memset(array_in, 0, M_MAX_VALUE * N_MAX_VALUE * sizeof(UINT16));
	N = params[0];
	readFifo(inputFIFOIds[0],inputFIFOAddrs[0], N * M_MAX_VALUE * sizeof(UINT16), (UINT8*)array_in);

	writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], M_MAX_VALUE * N_MAX_VALUE * sizeof(UINT16), (UINT8*)array_in);
}


void wrFile(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M[M_MAX_VALUE];
	UINT16 array[M_MAX_VALUE * N_MAX_VALUE];

	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)M);
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], M_MAX_VALUE * N_MAX_VALUE * sizeof(UINT16), (UINT8*)array);
}



/********** MLoop hierarchy *********/

void configM(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M;

	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&M);
	// Sending parameter's value.
	RTQueuePush_UINT32(RTInfoQueue, M);
}


void initMLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M;
	UINT16 line[M_MAX_VALUE];
	UINT16 line_out[M_MAX_VALUE];

	M = params[0];
	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)line);
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], M_MAX_VALUE * sizeof(UINT16), (UINT8*)line);

	writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], M * sizeof(UINT16), (UINT8*)line_out);
}


void f(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 inData;
	UINT16 outData;

	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&inData);

	outData = inData++;

	writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&outData);
}


void endMLoop(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 M;
	UINT16 line_in[M_MAX_VALUE];
	UINT16 line_out[M_MAX_VALUE];

	M = params[0];
	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], M * sizeof(UINT16), (UINT8*)line_in);

	writeFifo(outputFIFOIds[0],outputFIFOAddrs[0], M_MAX_VALUE * sizeof(UINT16), (UINT8*)line_out);
}


//******** Special actors ***********//
void RB0(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{

}

void RB1(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{

}

void broadcast(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{
	UINT16 inData;

	readFifo(inputFIFOIds[0],inputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&inData);

	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(UINT16), (UINT8*)&inData);
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], sizeof(UINT16), (UINT8*)&inData);
}

void M_in(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{

}

void line_in(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{

}

void line_out(UINT32 inputFIFOIds[],
			 UINT32 inputFIFOAddrs[],
			 UINT32 outputFIFOIds[],
			 UINT32 outputFIFOAddrs[],
			 UINT32 params[])
{

}
