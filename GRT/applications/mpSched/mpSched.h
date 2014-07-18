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
#include <platform_types.h>

#include "actors.h"

#define NVAL	10
#define MMAX 	12
#define NBITER 	10

//#define NVAL	20
//#define MMAX 	20
//#define NBITER 	10

extern UINT32 curVertexId;

//static UINT8 nValues[NBITER][MMAX] = {
//		{2,1},
//	};

void config(UINT8* inputFIFOs[],
			UINT8* outputFIFOs[],
			UINT32 params[])
{
//	UINT32 N;
	UINT32 NMAX = params[0];
	UINT32 N = params[1];
	int i;

//	N = NVAL;
	printf("Recv N=%d, vxId %d \n", N, curVertexId);

	UINT8* out_M = outputFIFOs[0];

	// Sending parameter's value.
	pushParam(curVertexId,1,&N);

//	memcpy(out_M, nValues[0], NMAX);
	for(i=0; i<N; i++)
		out_M[i] = 8;
}


void mFilter(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 NMAX = params[0];
	UINT32 N = params[1];

	UINT8* in_m = inputFIFOs[0];
	UINT8* out_m = outputFIFOs[0];

	//printf("Exec mFilter\n");

	memcpy(out_m, in_m, N);
}


void src(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 i,j;
	UINT32 N = params[0];
	UINT32 NBSAMPLES = params[1];

	float* out = (float*)outputFIFOs[0];

	//printf("Exec src\n");

	FILE* f;
	char file[100];

//	s//printf(file,"/home/jheulot/dev/mp-sched/input.dat");
//	f = fopen(file,"rb");
//	if(f == NULL){//printf("cannot open %s\n", file);abort();}
//	fread(out, sizeof(float), NBSAMPLES, f);
//	fclose(f);
//
//	for(i=1; i<N; i++){
//		memcpy(out+i*NBSAMPLES, out, NBSAMPLES*sizeof(float));
//	}
}

void snk(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 i,j;
	UINT32 N = params[0];
	UINT32 NBSAMPLES = params[1];

	float* in = (float*)inputFIFOs[0];

	BOOL test = TRUE;

	//printf("Exec snk\n");

//	float* outputCheck = OSAllocWorkingMemory(NBSAMPLES*sizeof(float));

	FILE* f;
	char file[100];

//	for(i=0; i<N; i++){
//		s//printf(file,"/home/jheulot/dev/mp-sched/output_%d_%d.dat", NBSAMPLES, nValues[0][i]);
//		f = fopen(file,"rb");
//		if(f == NULL){//printf("cannot open %s\n", file);abort();}
//		fread(outputCheck, sizeof(float), NBSAMPLES, f);
//		fclose(f);
//
//		for(j=0; j<NBSAMPLES; j++){
//			if(in[j+i*NBSAMPLES] != outputCheck[j]){
//				//printf("Error in (%d,%d), expected %f get %f\n",i,j,outputCheck[j],in[j]);
//				test = FALSE;
//				break;
//			}
//		}
//	}
//
//	if(test){
//		//printf("Passed\n");
//	}else{
//		//printf("Not Passed\n");
//	}
}

void setM(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT8* in_m = inputFIFOs[0];

	//printf("Exec setM\n");

	UINT32 M = in_m[0];

	// Sending parameter's value.
	pushParam(curVertexId,1,&M);
}

void initSwitch(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 M = params[0];
	UINT32 i;

	UINT8* out = outputFIFOs[0];

	out[0] = 0;
	for(i=1; i<M; i++){
		out[i] = 1;
	}
}

void switchFct(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 NBSAMPLES = params[0];

	UINT8 select = inputFIFOs[0][0];
	void *in0 = inputFIFOs[1];
	void *in1 = inputFIFOs[2];
	void *out = outputFIFOs[0];

	//printf("Exec switchFct\n");

	if(select == 0){
		memcpy(out, in0, NBSAMPLES*sizeof(float));
	}else{
//		memcpy(out, in1, NBSAMPLES*sizeof(float));
	}
}


void FIR(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 NBSAMPLES = params[0];

	float* in = (float*)inputFIFOs[0];
	float* out = (float*)outputFIFOs[0];

	//printf("Exec FIR\n");

	fir(in, out, NBSAMPLES);
}


